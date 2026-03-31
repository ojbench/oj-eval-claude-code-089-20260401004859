// Copyright (c) 2024 ACM Class, SJTU

namespace sjtu {

// Simple linked list node for free blocks
struct FreeBlock {
  int addr;
  int size;
  FreeBlock* next;
};

class BuddyAllocator {
public:
  /**
   * @brief Construct a new Buddy Allocator object with the given RAM size and
   * minimum block size.
   *
   * @param ram_size Size of the RAM. The address space is 0 ~ ram_size - 1.
   * @param min_block_size Minimum size of a block. The block size is 2^k where
   * k >= min_block_size.
   */
  BuddyAllocator(int ram_size, int min_block_size) {
    ram_size_ = ram_size;
    min_block_size_ = min_block_size;

    // Track allocated regions using a simple array
    max_regions_ = 10000;
    allocated_count_ = 0;
    allocated_addrs_ = new int[max_regions_];
    allocated_sizes_ = new int[max_regions_];
  }

  ~BuddyAllocator() {
    delete[] allocated_addrs_;
    delete[] allocated_sizes_;
  }

  /**
   * @brief Allocate a block with the given size at the minimum available
   * address.
   *
   * @param size The size of the block.
   * @return int The address of the block. Return -1 if the block cannot be
   * allocated.
   */
  int malloc(int size) {
    // Find the minimum address that is aligned to size and has space available
    for (int addr = 0; addr < ram_size_; addr += size) {
      if (is_range_free(addr, size)) {
        add_allocation(addr, size);
        return addr;
      }
    }
    return -1;
  }

  /**
   * @brief Allocate a block with the given size at the given address.
   *
   * @param addr The address of the block.
   * @param size The size of the block.
   * @return int The address of the block. Return -1 if the block cannot be
   * allocated.
   */
  int malloc_at(int addr, int size) {
    if (addr < 0 || addr + size > ram_size_) {
      return -1;
    }

    if (!is_range_free(addr, size)) {
      return -1;
    }

    add_allocation(addr, size);
    return addr;
  }

  /**
   * @brief Deallocate a block with the given size at the given address.
   *
   * @param addr The address of the block. It is ensured that the block is
   * allocated before.
   * @param size The size of the block.
   */
  void free_at(int addr, int size) {
    // Find and remove the allocation
    for (int i = 0; i < allocated_count_; i++) {
      if (allocated_addrs_[i] == addr && allocated_sizes_[i] == size) {
        // Remove by shifting
        for (int j = i; j < allocated_count_ - 1; j++) {
          allocated_addrs_[j] = allocated_addrs_[j + 1];
          allocated_sizes_[j] = allocated_sizes_[j + 1];
        }
        allocated_count_--;
        return;
      }
    }
  }

private:
  int ram_size_;
  int min_block_size_;
  int max_regions_;
  int allocated_count_;
  int* allocated_addrs_;
  int* allocated_sizes_;

  bool is_range_free(int addr, int size) {
    int end = addr + size;
    for (int i = 0; i < allocated_count_; i++) {
      int alloc_start = allocated_addrs_[i];
      int alloc_end = alloc_start + allocated_sizes_[i];

      // Check for overlap
      if (!(end <= alloc_start || addr >= alloc_end)) {
        return false;
      }
    }
    return true;
  }

  void add_allocation(int addr, int size) {
    allocated_addrs_[allocated_count_] = addr;
    allocated_sizes_[allocated_count_] = size;
    allocated_count_++;
  }
};

} // namespace sjtu
