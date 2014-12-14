#include "database.hpp"

#include <cassert>
#include <cmath>
#include <cstring>

namespace sid = foonathan::string_id;

class sid::map_database::node_list
{    
    struct node
    {
        hash_type hash;
        node *next;
        
        node(const char *str, hash_type h, node *next) noexcept
        : hash(h), next(next)
        {
            void* mem = this;
            std::strcpy(static_cast<char*>(mem) + sizeof(node), str);
        }
        
        const char* get_str() const noexcept
        {
            const void *mem = this;
            return static_cast<const char*>(mem) + sizeof(node);
        }
    };
public:
    node_list() noexcept
    : head_(nullptr) {}
    
    ~node_list() noexcept
    {
        auto cur = head_;
        while (cur)
        {
            auto next = cur->next;
            ::operator delete(cur);
            cur = next;
        }
    }
    
    // inserts new node, checks for collisions and updates number of nodes
    bool insert(std::size_t &size, hash_type hash, const char *str, std::size_t length)
    {
        if (!head_)
        {
            auto mem = ::operator new(sizeof(node) + length + 1);
            head_ = ::new(mem) node(str, hash, nullptr);
            ++size;
        }
        else
        {
            auto inserted = false;
            auto pos = insert_pos(hash, inserted);
            if (inserted)
                return std::strcmp(pos.first->get_str(), str) == 0;
            auto mem = ::operator new(sizeof(node) + length + 1);
            pos.first->next = ::new(mem) node(str, hash, pos.second);
            ++size;
        }
        return true;
    }
    
    // inserts all nodes into new buckets, this list is empty afterwards
    void rehash(node_list *buckets, std::size_t size) noexcept
    {
        auto cur = head_;
        while (cur)
        {
            auto next = cur->next;
            auto& list = buckets[cur->hash % size];
            if (!list.head_)
            {
                list.head_ = cur;
                cur->next = nullptr;
            }
            else
            {
                auto inserted = false;
                auto pos = list.insert_pos(cur->hash, inserted);
                assert(!insert && "element can't be there already");
                pos.first->next = cur;
                cur->next = pos.second;
            }
            cur = next;
        }
        head_ = nullptr;
    }
    
    // returns element with hash, there must be one
    const char* lookup(hash_type h) const noexcept
    {
        auto cur = head_;
        while (cur->hash < h)
            cur = cur->next;
        return cur->get_str();
    }
    
private:
    // assumes head isn't nullptr
    std::pair<node*, node*> insert_pos(hash_type hash, bool &inserted)
    {
        auto cur = head_->next, prev = head_;
        while (cur)
        {
            if (cur->hash < hash)
            {
                prev = cur;
                cur = cur->next;
            }
            else if (cur->hash == hash)
            {
                inserted = true;
                return std::make_pair(cur, cur);
            }
            else
                break;
        }
        return std::make_pair(prev, cur);
    }
    
    node *head_;
};

sid::map_database::map_database(std::size_t size, double max_load_factor)
: buckets_(new node_list[size]),
  no_items_(0u), no_buckets_(size),
  max_load_factor_(max_load_factor),
  next_resize_(std::floor(no_buckets_ * max_load_factor_))
{}

sid::map_database::~map_database() noexcept {}

bool sid::map_database::insert(hash_type hash, const char *str, std::size_t length)
{
    static constexpr auto growth_factor = 2;
    if (no_items_ + 1 >= next_resize_)
    {
        auto new_size = growth_factor * no_buckets_;
        auto buckets = new node_list[new_size]();
        auto end = buckets_.get() + no_buckets_;
        for (auto list = buckets_.get(); list != end; ++list)
            list->rehash(buckets, new_size);
        buckets_.reset(buckets);
        no_buckets_ = new_size;
        next_resize_ = std::floor(no_buckets_ * max_load_factor_);
    }
    return buckets_[hash % no_buckets_].insert(no_items_, hash, str, length);
}

const char* sid::map_database::lookup(hash_type hash) const noexcept
{
    return buckets_[hash % no_buckets_].lookup(hash);
}
