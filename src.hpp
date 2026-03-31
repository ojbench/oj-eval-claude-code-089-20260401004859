// Copyright (c) 2024 ACM Class, SJTU

namespace sjtu {

class BuddyAllocator {
public:
  BuddyAllocator(int ram_size, int min_block_size) {
    ram_size_ = ram_size;
    min_block_size_ = min_block_size;

    // Use a bitmap to track allocation status at min_block_size granularity
    num_blocks_ = ram_size / min_block_size;
    allocated_ = new bool[num_blocks_];
    for (int i = 0; i < num_blocks_; i++) {
      allocated_[i] = false;
    }
  }

  ~BuddyAllocator() {
    delete[] allocated_;
  }

  int malloc(int size) {
    int num_blocks = size / min_block_size_;

    // Find the first aligned position that has enough free space
    for (int start = 0; start < num_blocks_; start += num_blocks) {
      int addr = start * min_block_size_;
      if (is_range_free(addr, size)) {
        mark_range(addr, size, true);
        return addr;
      }
    }
    return -1;
  }

  int malloc_at(int addr, int size) {
    if (addr < 0 || addr + size > ram_size_) {
      return -1;
    }

    if (!is_range_free(addr, size)) {
      return -1;
    }

    mark_range(addr, size, true);
    return addr;
  }

  void free_at(int addr, int size) {
    mark_range(addr, size, false);
  }

private:
  int ram_size_;
  int min_block_size_;
  int num_blocks_;
  bool* allocated_;

  bool is_range_free(int addr, int size) {
    int start_block = addr / min_block_size_;
    int num_blocks = size / min_block_size_;

    for (int i = start_block; i < start_block + num_blocks; i++) {
      if (allocated_[i]) {
        return false;
      }
    }
    return true;
  }

  void mark_range(int addr, int size, bool alloc) {
    int start_block = addr / min_block_size_;
    int num_blocks = size / min_block_size_;

    for (int i = start_block; i < start_block + num_blocks; i++) {
      allocated_[i] = alloc;
    }
  }
};

} // namespace sjtu
