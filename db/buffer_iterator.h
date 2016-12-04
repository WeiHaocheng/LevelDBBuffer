#ifndef STORAGE_LEVELDB_DB_BUFFER_ITERATOR_H_
#define STORAGE_LEVELDB_DB_BUFFER_ITERATOR_H_

namespace leveldb{
    class BufferNodeIterator : public Iterator{
    private:
        BufferNode node_;
       Iterator* iter;   //对应sst的iterator
        InternalKeyComparator icmp;
    public:
        BufferNodeIterator(VersionSet* vset,BufferNode node);
        //在vset中拿到table_cache，icmp
        //根据传入的ssd_table_cache打开Table，然后拿到这个table的iter,,具体了流程参考table_cache的newiterator代码
        // ssd_table_cache是为ssd中的文件建立的table_cache

        virtual bool Valid() const ;
        //return iter->Valid() && node_->largest >= iter->key()  && node_->smallest <= iter->key()

        virtual Slice key() const;
        // return iter->key();

        virtual Slice value() const;
        //同上

        virtual Status status() const { return Status::OK(); }

        virtual void Next();
       // 如果还在buffernode的范围内，将指针右移
        // if(iter->key() < =node_->largest) iter->next();

        virtual void Prev();
        //同上

        virtual void Seek(const Slice& target);
        // 将指针移到大于等于target的第一个位置，如果没有，将指针指向最后一个数,如果小于最小，指向第一个数
        // iter->Seek(target)
        //if(iter->key()>node_->largest)
        // SeekTolast()
        //if(iter->key()>node_->largest)
        // SeekToFirst()

        virtual void SeekToFirst();
        //iter->Seek(node_->smallest)
        //assert(iter中没有key等于smallest)
        virtual void SeekToLast();
      //iter->Seek(node_->largest)
        // assert(类似)

        virtual ~BufferNodeIterator();
       //delete iter
    };

    extern Iterator* NewBufferIterator(VersionSet* vset,Buffer* buffer);
    /*
     std::vector<Iterator*> list;
     assert(buffer==NULL)
     for(int i=0;i<buffer->nodes.size();i++){
     	 BufferNodeIterator* ptr = new BufferNodeIterator(vset,buffer->nodes[i]);
     	 list.push_back(ptr);
     }
     Iterator* buffer_iter =
      NewMergingIterator(vset->icmp, &list[0], list.size());
      return buffer_iter;
     */
}
#endif
