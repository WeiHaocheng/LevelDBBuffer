// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_VERSION_EDIT_H_
#define STORAGE_LEVELDB_DB_VERSION_EDIT_H_

#include <set>
#include <utility>
#include <vector>
#include "db/dbformat.h"

namespace leveldb {

class VersionSet;

struct Buffer;

//whc change
struct FileMetaData {
  int refs;
  int allowed_seeks;          // Seeks allowed until compaction
  uint64_t number;
  uint64_t file_size;         // File size in bytes
  InternalKey smallest;       // Smallest internal key served by table
  InternalKey largest;        // Largest internal key served by table
  Buffer* buffer;               //whc add

  FileMetaData() : refs(0), allowed_seeks(1 << 30), file_size(0),buffer(NULL) { }
  
  //~FileMetaData(){delete buffer;}
};

//whc add
struct BufferTable{
	int refs;
	uint64_t number;

	BufferTable(uint64_t n):number(n),refs(0){}
};

struct BufferNode{
	InternalKey smallest;
	InternalKey largest;
	uint64_t number;
	uint64_t size;
	uint64_t sequence;
    uint64_t filesize;

	BufferNode(InternalKey& s,InternalKey& l,uint64_t n,uint64_t si,uint64_t se,uint64_t fs):smallest(s),
			largest(l),
			number(n),
			size(si),
			sequence(se),
            filesize(fs){}
};

struct Buffer{
	std::vector<BufferNode> nodes;
	InternalKey smallest;
	InternalKey largest;
	uint64_t size;

	Buffer(){}

};

struct BufferNodeEdit{
	InternalKey smallest;
	InternalKey largest;
	uint64_t snumber;  //source number
	uint64_t dnumber; // destination number
	uint64_t size;
    uint64_t filesize;
	bool inend; //true is in end buffer false is not
};

class VersionEdit {
 public:
  VersionEdit() { Clear(); }
  ~VersionEdit() { }

  void Clear();

  void SetComparatorName(const Slice& name) {
    has_comparator_ = true;
    comparator_ = name.ToString();
  }
  void SetLogNumber(uint64_t num) {
    has_log_number_ = true;
    log_number_ = num;
  }
  void SetPrevLogNumber(uint64_t num) {
    has_prev_log_number_ = true;
    prev_log_number_ = num;
  }
  void SetNextFile(uint64_t num) {
    has_next_file_number_ = true;
    next_file_number_ = num;
  }
  void SetLastSequence(SequenceNumber seq) {
    has_last_sequence_ = true;
    last_sequence_ = seq;
  }
  void SetCompactPointer(int level, const InternalKey& key) {
    compact_pointers_.push_back(std::make_pair(level, key));
  }

  // Add the specified file at the specified number.
  // REQUIRES: This version has not been saved (see VersionSet::SaveTo)
  // REQUIRES: "smallest" and "largest" are smallest and largest keys in file
  void AddFile(int level, uint64_t file,
               uint64_t file_size,
               const InternalKey& smallest,
               const InternalKey& largest) {
    FileMetaData f;
    f.number = file;
    f.file_size = file_size;
    f.smallest = smallest;
    f.largest = largest;
    new_files_.push_back(std::make_pair(level, f));
  }

  // Delete the specified "file" from the specified "level".
  void DeleteFile(int level, uint64_t file) {
    deleted_files_.insert(std::make_pair(level, file));
  }

  //whc add
  void AddBufferNode(int level,uint64_t snumber,uint64_t ssize,
		  uint64_t dnumber,
		  uint64_t size,
		  InternalKey& smallest,
		  InternalKey& largest,
		 bool inend ){
	  BufferNodeEdit b;
	  InternalKey fill;
      b.snumber = snumber;
	  b.dnumber = dnumber;
	  b.size = size;
	  
      if(!inend)
        b.smallest = smallest;
	  else
        b.smallest = fill;
          
      b.largest = largest;
	  b.inend = inend;
      b.filesize = ssize;
	  new_buffer_nodes.push_back(std::make_pair(level, b));
  }

  void EncodeTo(std::string* dst) const;
  Status DecodeFrom(const Slice& src);

  std::string DebugString() const;

 private:
  friend class VersionSet;

  typedef std::set< std::pair<int, uint64_t> > DeletedFileSet;

  std::string comparator_;
  uint64_t log_number_;
  uint64_t prev_log_number_;
  uint64_t next_file_number_;
  SequenceNumber last_sequence_;
  bool has_comparator_;
  bool has_log_number_;
  bool has_prev_log_number_;
  bool has_next_file_number_;
  bool has_last_sequence_;

  std::vector< std::pair<int, InternalKey> > compact_pointers_;
  DeletedFileSet deleted_files_;
  std::vector< std::pair<int, FileMetaData> > new_files_;
  //whc add
  std::vector< std::pair<int, BufferNodeEdit> > new_buffer_nodes;
  std::vector<int> reset_end_levels;
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_VERSION_EDIT_H_
