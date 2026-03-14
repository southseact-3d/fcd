#ifndef MESH_FUNCTIONAL_H
#define MESH_FUNCTIONAL_H

#include <algorithm>
#include <future>


namespace MeshCore
{
template<class Iter, class Pred>
static void parallel_sort(Iter begin, Iter end, Pred comp, int threads)
{
    if (threads < 2 || end - begin < 2) {
        std::sort(begin, end, comp);
    }
    else {
        Iter mid = begin + (end - begin) / 2;
        if (threads == 2) {
            auto future = std::async(parallel_sort<Iter, Pred>, begin, mid, comp, threads / 2);
            std::sort(mid, end, comp);
            future.wait();
        }
        else {
            auto a = std::async(
                std::launch::async,
                parallel_sort<Iter, Pred>,
                begin,
                mid,
                comp,
                threads / 2
            );
            auto b
                = std::async(std::launch::async, parallel_sort<Iter, Pred>, mid, end, comp, threads / 2);
            a.wait();
            b.wait();
        }
        std::inplace_merge(begin, mid, end, comp);
    }
}

}  // namespace MeshCore


#endif  // MESH_FUNCTIONAL_H
