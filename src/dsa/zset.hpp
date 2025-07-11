#ifndef ZSET_HPP
#define ZSET_HPP

#include "avl.hpp" // include the avl tree header file
#include "hashtable.hpp" // include the custom hash table header file
#include "thread_pool.hpp" // include the thread pool header file
#include <memory> // include memory management utilities like unique_ptr
#include <mutex>
#include <future>
using ZNode = AVLNode<std::string, double>;

class ZSet { // define the zset class
public:
    AVLTree<std::string, double> tree; // avl tree to maintain ordered scores
    HMap<std::string, ZNode*> hash; // Assuming HMap<K, V> uses string keys
    ThreadPool thread_pool_; // thread pool for handling asynchronous tasks
    std::vector<std::shared_ptr<ZNode>> nodes_; // Keeps ZNode alive
    mutable std::shared_mutex zset_mutex_;
    explicit ZSet(size_t threads = 4) : thread_pool_(threads) {} // initialize thread pool with a default of 4 worker threads


    ZNode* lookup(std::string_view name) {
        std::shared_lock lock(zset_mutex_);
        std::cerr << "Looking up: " << name << std::endl;
        ZNode** node_ptr = hash.find(std::string(name));
        return (node_ptr && *node_ptr) ? *node_ptr : nullptr;
    }
    
    bool add_internal(std::string_view name, double score) {
        std::unique_lock lock(zset_mutex_);  
        if (ZNode* node = lookup(name)) {
            update_score(node, score);
            return false;
        }
    
        // Create a shared pointer to ensure ownership
        auto node = std::make_shared<ZNode>(std::string(name), score);
    
        if (node->get_key().empty()) {
            std::cerr << "ERROR: Created node has empty key after construction!" << std::endl;
            return false;
        }
    
        std::cerr << "Created node: " << node->get_key() << " with score: " << node->get_value() << std::endl;
    
        nodes_.emplace_back(node);  
        hash.insert(node->get_key(), node.get()); 
        tree.set(node->get_key(), score);
    
        std::cerr << "Added node successfully: " << node->get_key() << std::endl;
        return true;
    }
    
    

    // std::future<ZNode*> pop(std::string_view name) { // asynchronously removes and returns a node
    //     return thread_pool_.enqueue([this, name] { // enqueues the pop operation in the thread pool
    //         return pop_internal(name); // calls the internal pop function
    //     });
    // }

    ZNode* pop_internal(std::string_view name) {
        std::unique_lock lock(zset_mutex_);
        ZNode** node_ptr = hash.find(std::string(name));
        if (!node_ptr || !*node_ptr) return nullptr;

        ZNode* node = *node_ptr;
        hash.remove(std::string(name));
        tree.del(node->get_key());
        lock.unlock();
        {
            nodes_.erase(std::remove_if(nodes_.begin(), nodes_.end(),
                        [node](const std::shared_ptr<ZNode>& n) { return n.get() == node; }),
                        nodes_.end());
            }
        return node;
    }

    
    bool update_score(ZNode* node, double new_score) {
        std::unique_lock lock(zset_mutex_);  
        if (!node) {
            std::cerr << "update_score: Received a nullptr!" << std::endl;
            return false;
        }
    
        std::string key = node->get_key();
        if (key.empty()) {
            std::cerr << "update_score: Node key is empty!" << std::endl;
            return false;
        }
    
        ZNode** node_ptr = hash.find(key);
        if (!node_ptr || !*node_ptr) {
            std::cerr << "update_score: Key not found in hash: " << key << std::endl;
            return false; 
        }
    
        tree.del(key);
        node->set_value(new_score);
        tree.set(key, new_score);
        
        return true; 
    }
    
    bool remove_internal(std::string_view name) {
        std::unique_lock lock(zset_mutex_);
        ZNode** node_ptr = hash.find(std::string(name));
        if (!node_ptr || !*node_ptr) return false;
    
        ZNode* node = *node_ptr;
        hash.remove(std::string(name));  
        tree.del(node->get_key());
    
        nodes_.erase(std::remove_if(nodes_.begin(), nodes_.end(),
                     [node](const std::shared_ptr<ZNode>& n) { return n.get() == node; }),
                     nodes_.end());
    
        return true;
    }
    

    // std::future<bool> remove(std::string_view name) {
    //     return thread_pool_.enqueue([this, name] {
    //         return remove_internal(name);
    //     });
    // }
    
    ZNode* query(double score, std::string_view name, int64_t offset) {
        std::shared_lock lock(zset_mutex_);  
        return tree.exists(std::string(name)) ? lookup(name) : nullptr;
    }

    ~ZSet() {
        hash.clear();  
        nodes_.clear();  
        thread_pool_.wait_for_tasks(); 
        thread_pool_.shutdown();
    }
    
    
};

#endif