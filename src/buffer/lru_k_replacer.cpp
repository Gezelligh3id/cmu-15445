//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"

namespace bustub {

LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool { 
    std::scoped_lock<std::mutex> lock(latch_);
    if(curr_size_ == 0) {
        return false;
    }
    for(auto item = history_list_.rbegin(); item != history_list_.rend(); item++) {
        auto tmp = *item;
        if(is_evictable_[tmp]) {
            *frame_id = tmp;
            access_record_[tmp] = 0;
            is_evictable_[tmp] = false;
            curr_size_ --;
            history_list_.erase(history_map_[tmp]);
            history_map_.erase(tmp);
            return true;
        }
    }
    for(auto item = cache_list_.rbegin(); item != cache_list_.rend(); item++) {
        auto tmp = *item;
        if(is_evictable_[tmp]) {
            *frame_id = tmp;
            access_record_[tmp] = 0;
            is_evictable_[tmp] = false;
            curr_size_ --;
            cache_list_.erase(cache_map_[tmp]);
            cache_map_.erase(tmp);
            return true;
        }
    }
    return false; 
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id) {
    std::scoped_lock<std::mutex> lock(latch_);
    if (frame_id > static_cast<int>(replacer_size_)) {
        throw std::exception();
    }
    access_record_[frame_id]++;
    if(access_record_[frame_id] == k_) {
        history_list_.erase(history_map_[frame_id]);
        history_map_.erase(frame_id);
        cache_list_.push_front(frame_id);
        cache_map_[frame_id] = cache_list_.begin();
    } else if (access_record_[frame_id] > k_) {
        if (cache_map_.count(frame_id) != 0) {
            cache_list_.erase(cache_map_[frame_id]);
            cache_map_.erase(frame_id);
        }
        cache_list_.push_front(frame_id);
        cache_map_[frame_id] = cache_list_.begin();
    } else {
        if (history_map_.count(frame_id) == 0) {
            history_list_.push_front(frame_id);
            history_map_[frame_id] = history_list_.begin();
        }
    }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
    std::scoped_lock<std::mutex> lock(latch_);
    if (access_record_[frame_id] == 0) {
        return;
    }
    if (set_evictable && !is_evictable_[frame_id]) {
        curr_size_++;
    }
    if (!set_evictable && is_evictable_[frame_id]) {
        curr_size_--;
    }
    is_evictable_[frame_id] = set_evictable;
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
    std::scoped_lock<std::mutex> lock(latch_);
    if (access_record_[frame_id] == 0) {
        return;
    }
    if (!is_evictable_[frame_id]) {
        throw std::exception();
    }
    if (access_record_[frame_id] < k_) {
        history_list_.erase(history_map_[frame_id]);
        history_map_.erase(frame_id);
    } else {
        cache_list_.erase(cache_map_[frame_id]);
        cache_map_.erase(frame_id);
    }
    is_evictable_[frame_id] = false;
    access_record_[frame_id] = 0;
    curr_size_--;
}

auto LRUKReplacer::Size() -> size_t { 
    std::scoped_lock<std::mutex> lock(latch_);
    return curr_size_; 
}

}  // namespace bustub
