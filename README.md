
## 1.Usage
```
./vbenchmark help
description
Usage:
  name [OPTION...]

      --version              print version
      --normalize            normalize vectors
      --length               output vector length
      --path arg             file/dir (default: "")
      --otype arg            stdout/file/leveldb/rocksdb (default: "")
      --itype arg            leveldb/rocksdb (default: "")
      --btype arg            leveldb/rocksdb/file/generate (default: "")
      --etype arg            hnswlib/faiss (default: "")
      --dtype arg            l2/inner (default: "")
      --stype arg            hnsw/brute (default: "")
      --Mvalue arg           M value (default: 0)
      --ef_construction arg  ef_construction (default: 0)
      --dim arg              dimension (default: 0)
      --count arg            vector count (default: 0)
      --limit arg            read limit (default: 999999999)
      --searchk arg          search k nearest neighbors (default: 0)
      --sample arg           sample count (default: 0)


example: 
./vbenchmark write --otype=stdout --dim=8 --length --count=100 --normalize
./vbenchmark write --otype=file --dim=8 --length --count=100 --normalize
./vbenchmark write --otype=leveldb --dim=8 --count 100 --normalize
./vbenchmark write --otype=rocksdb --dim=8 --count 100 --normalize
./vbenchmark read --itype=leveldb --path=ldb --limit=10
./vbenchmark read --itype=rocksdb --path=rdb --limit=10
./vbenchmark build --btype=generate --dim=1024 --normalize --etype=hnswlib --dtype=inner --stype=hnsw --Mvalue=16 --ef_construction=200  --count=1000
./vbenchmark query --path=./hnswlib.dim_1024.cnt_1000.ts_1754740321381 --dim=1024 --etype=hnswlib --dtype=inner --stype=hnsw --Mvalue=16 --ef_construction=200 --count=1000
./vbenchmark prepare --dim=1024 --etype=hnswlib --dtype=inner --Mvalue=16 --ef_construction=200 --count=10000
./vbenchmark recall --dim=1024 --etype=hnswlib --dtype=inner --Mvalue=16 --ef_construction=200 --sample=10 --count=10000 --searchk=500 --path=./prepare.dim_1024.cnt_10000.ts_1754799214767/
./vbenchmark pr --dim=1024 --etype=hnswlib --dtype=inner --Mvalue=16 --ef_construction=200 --count=10000 --searchk=500 --sample=10 --path=./prepare
```

## 2.Recall Test

### 2.1 dim

### 2.2 M

### 2.3 ef_construction

### 2.4 normalize


## 3.GPU Accelerate
```
生成随机向量数据...

=== CPU索引 ===
CPU索引构建时间: 1.7870秒
CPU搜索时间: 6.9660秒

=== GPU索引 ===
GPU索引构建时间: 0.5073秒
GPU搜索时间: 0.4248秒

=== 检索结果示例 ===
查询数量: 1000, 每个查询返回结果数: 10
第一个查询的CPU结果 (索引/距离):
425675: 140.387390
244761: 141.498657
713383: 141.529297
334175: 142.016846
776248: 142.844421
495490: 142.968384
809254: 143.071716
431121: 143.583618
750695: 144.118652
55664: 144.341675

第一个查询的GPU结果 (索引/距离):
425675: 140.387207
1425675: 140.387207
1244761: 141.498169
244761: 141.498169
713383: 141.529785
1713383: 141.529785
1334175: 142.016907
334175: 142.016907
776248: 142.844910
1776248: 142.844910

=== 性能总结 ===
索引构建加速比: 3.52x
搜索加速比: 16.40x
```

