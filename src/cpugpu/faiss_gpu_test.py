# 编写python代码，使用faiss进行向量检索
# 生成1000000条1024维随机的向量，使用cpu和gpu分别建立索引
# 随机生成1000条向量，使用faiss-cpu, faiss-gpu进行向量检索，返回最相似的10条向量
# 打印检索结果，检索耗时

import numpy as np
import faiss
import time

# 生成1000000条1024维随机向量
print("生成随机向量数据...")
d = 1024  # 向量维度
nb = 1000000  # 数据库大小
nq = 1000  # 查询数量

# 使用固定随机种子确保可复现性
np.random.seed(1234) 
xb = np.random.random((nb, d)).astype('float32')
xq = np.random.random((nq, d)).astype('float32')


# 获取内存和显存占用的函数
def get_memory_usage():
    import psutil
    process = psutil.Process()
    mem_info = process.memory_info()
    return mem_info.rss / (1024 ** 2)  # 返回MB

def get_gpu_memory_usage():
    import pynvml
    pynvml.nvmlInit()
    handle = pynvml.nvmlDeviceGetHandleByIndex(0)
    info = pynvml.nvmlDeviceGetMemoryInfo(handle)
    return info.used / (1024 ** 2)  # 返回MB

# CPU索引建立和搜索
print("\n=== CPU索引 ===")
index_cpu = faiss.IndexFlatL2(d)  # 使用L2距离的Flat索引

mem_before_cpu = get_memory_usage()
start = time.time()
index_cpu.add(xb)  # 添加向量到索引
build_time_cpu = time.time() - start
mem_after_cpu = get_memory_usage()
mem_usage_cpu = mem_after_cpu - mem_before_cpu
print(f"CPU索引构建时间: {build_time_cpu:.4f}秒, 内存占用: {mem_usage_cpu:.2f} MB")

start = time.time()
D_cpu, I_cpu = index_cpu.search(xq, 10)  # 搜索每个查询的10个最近邻
search_time_cpu = time.time() - start
print(f"CPU搜索时间: {search_time_cpu:.4f}秒")

# GPU索引建立和搜索
print("\n=== GPU索引 ===")
res = faiss.StandardGpuResources()  # 获取GPU资源

gpu_mem_before = get_gpu_memory_usage()
# 将CPU索引转移到GPU
index_gpu = faiss.index_cpu_to_gpu(res, 0, index_cpu)

start = time.time()
index_gpu.add(xb)  # 添加向量到GPU索引
build_time_gpu = time.time() - start
gpu_mem_after = get_gpu_memory_usage()
gpu_mem_usage = gpu_mem_after - gpu_mem_before
print(f"GPU索引构建时间: {build_time_gpu:.4f}秒, 显存占用: {gpu_mem_usage:.2f} MB")

start = time.time()
D_gpu, I_gpu = index_gpu.search(xq, 10)  # GPU搜索
search_time_gpu = time.time() - start
print(f"GPU搜索时间: {search_time_gpu:.4f}秒")

# 打印部分结果
print("\n=== 检索结果示例 ===")
print(f"查询数量: {nq}, 每个查询返回结果数: 10")
print(f"第一个查询的CPU结果 (索引/距离):")
for i in range(10):
    print(f"{I_cpu[0][i]}: {D_cpu[0][i]:.6f}")

print(f"\n第一个查询的GPU结果 (索引/距离):")
for i in range(10):
    print(f"{I_gpu[0][i]}: {D_gpu[0][i]:.6f}")

# 性能比较
print("\n=== 性能总结 ===")
print(f"索引构建加速比: {build_time_cpu/build_time_gpu:.2f}x")
print(f"搜索加速比: {search_time_cpu/search_time_gpu:.2f}x")

# 新增：计算并打印每条查询的平均时间
avg_time_cpu = search_time_cpu * 1000 / nq  # 转换为毫秒
avg_time_gpu = search_time_gpu * 1000 / nq  # 转换为毫秒
print(f"CPU平均每条查询时间: {avg_time_cpu:.4f} 毫秒")
print(f"GPU平均每条查询时间: {avg_time_gpu:.4f} 毫秒")

# 打印内存/显存使用总结
print("\n=== 内存/显存使用总结 ===")
print(f"CPU索引内存占用: {mem_usage_cpu:.2f} MB")
print(f"GPU索引显存占用: {gpu_mem_usage:.2f} MB")

'''
生成随机向量数据...

=== CPU索引 ===
CPU索引构建时间: 1.8001秒, 内存占用: 3906.50 MB
CPU搜索时间: 4.5427秒

=== GPU索引 ===
GPU索引构建时间: 0.5159秒, 显存占用: 9604.19 MB
GPU搜索时间: 0.4356秒

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
索引构建加速比: 3.49x
搜索加速比: 10.43x
CPU平均每条查询时间: 4.5427 毫秒
GPU平均每条查询时间: 0.4356 毫秒

=== 内存/显存使用总结 ===
CPU索引内存占用: 3906.50 MB
GPU索引显存占用: 9604.19 MB
'''
