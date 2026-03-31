// Copyright (c) 2024 ACM Class, SJTU

namespace sjtu {

// Interval node for tracking free ranges
struct Interval {
  int start;
  int end;  // exclusive
  Interval* next;

  Interval(int s, int e) : start(s), end(e), next(nullptr) {}
};

class BuddyAllocator {
public:
  BuddyAllocator(int ram_size, int min_block_size) {
    ram_size_ = ram_size;
    min_block_size_ = min_block_size;

    // Initially, entire RAM is one free interval
    free_list_ = new Interval(0, ram_size);
  }

  ~BuddyAllocator() {
    Interval* curr = free_list_;
    while (curr) {
      Interval* next = curr->next;
      delete curr;
      curr = next;
    }
  }

  int malloc(int size) {
    // Find the first free interval that can accommodate size at an aligned address
    Interval* prev = nullptr;
    Interval* curr = free_list_;

    while (curr) {
      // Find the first aligned address in this interval
      int aligned_start = curr->start;
      if (aligned_start % size != 0) {
        aligned_start = ((aligned_start / size) + 1) * size;
      }

      // Check if we can allocate at aligned_start
      if (aligned_start + size <= curr->end) {
        // Allocate here
        allocate_in_interval(prev, curr, aligned_start, size);
        return aligned_start;
      }

      prev = curr;
      curr = curr->next;
    }

    return -1;  // No suitable space found
  }

  int malloc_at(int addr, int size) {
    if (addr < 0 || addr + size > ram_size_) {
      return -1;
    }

    // Find the interval containing addr
    Interval* prev = nullptr;
    Interval* curr = free_list_;

    while (curr) {
      if (addr >= curr->start && addr + size <= curr->end) {
        // Can allocate here
        allocate_in_interval(prev, curr, addr, size);
        return addr;
      }
      prev = curr;
      curr = curr->next;
    }

    return -1;  // Address not in a free interval
  }

  void free_at(int addr, int size) {
    // Insert the freed interval and merge with adjacent free intervals
    int start = addr;
    int end = addr + size;

    Interval* prev = nullptr;
    Interval* curr = free_list_;

    // Find the position to insert
    while (curr && curr->start < start) {
      prev = curr;
      curr = curr->next;
    }

    // Check if we can merge with prev
    bool merged_prev = false;
    if (prev && prev->end == start) {
      // Merge with prev
      prev->end = end;
      merged_prev = true;

      // Check if we can also merge with next
      if (curr && curr->start == end) {
        prev->end = curr->end;
        prev->next = curr->next;
        delete curr;
      }
    } else if (curr && curr->start == end) {
      // Merge with curr
      curr->start = start;
    } else {
      // Insert as a new interval
      Interval* new_interval = new Interval(start, end);
      new_interval->next = curr;
      if (prev) {
        prev->next = new_interval;
      } else {
        free_list_ = new_interval;
      }
    }
  }

private:
  int ram_size_;
  int min_block_size_;
  Interval* free_list_;  // Sorted list of free intervals

  void allocate_in_interval(Interval* prev, Interval* curr, int addr, int size) {
    int start = addr;
    int end = addr + size;

    if (start == curr->start && end == curr->end) {
      // Remove entire interval
      if (prev) {
        prev->next = curr->next;
      } else {
        free_list_ = curr->next;
      }
      delete curr;
    } else if (start == curr->start) {
      // Shrink from the start
      curr->start = end;
    } else if (end == curr->end) {
      // Shrink from the end
      curr->end = start;
    } else {
      // Split into two intervals
      int old_end = curr->end;
      curr->end = start;

      Interval* new_interval = new Interval(end, old_end);
      new_interval->next = curr->next;
      curr->next = new_interval;
    }
  }
};

} // namespace sjtu
