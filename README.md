<div align="center">
  <img src="https://github.com/lionell/labs/blob/master/parallel_prog/docs/img/googlepagerank.jpg" />
</div>

Implementation of [PageRank](https://en.wikipedia.org/wiki/PageRank) algorithm, using
different parallelization approaches. For now there are three of them available:
Serial(no parallelization), OpenMP(shared memory), MPI(communication via network).
We are going to benchmark them on different datasets.

## PageRank

PageRank is an algorithm used by Google Search to rank websites in their search engine results.
PageRank was named after Larry Page, one of the founders of Google.
PageRank is a way of measuring the importance of website pages. According to Google:

> PageRank works by counting the number and quality of links to a page to determine
> a rough estimate of how important the website is. The underlying assumption is that
> more important websites are likely to receive more links from other websites.

It is not the only algorithm used by Google to order search engine results,
but it is the first algorithm that was used by the company, and it is the best-known.

## Algorithm

There are different approaches of PageRank evaluation. They mostly differ in underlying
data structure(there is also MapReduce version). Method we used is great for distributed
computing, because we can minimize data transfered via network, and gain benefit from
shared memory.

For now we are going to call graph vertecies - pages. For each page we store list of
in-links. Eg. [3, 0, 1] means that our page has in-links from pages 0, 1 and 3.
Also we need to store amount of out-links for every page. Having these let's take a
look how we can compute PageRank efficiently.

Basically all approaches to computed PageRank are **iterative**. Let's divide one iteration
into three parts: PR from pages, PR from dangling pages and PR from random jumps.

### Pages

To compute PR from pages, we need to spread old PR(from previous iteration) across all the
pages linked with current. For example, if there are links from page 1 to pages 0, 2, 4, then
each of these pages will receive additional `PR[0] / 3` PageRank.

As you can see, we can use our data representation to compute this part. To compute PR of
some particular page, we can sum PRs of all in-pages divided by out link count for each page.

```
PR1[i] = PR[in_links[0]] / out_link_cnts[in_links[0]] + ... + PR[in_links[k]] / out_link_cnts[in_links[k]]
```

### Dangling pages

Dangling pages - are pages with no out-links. It's obvious that one will leave this page at some point
of time, and go to some random page. That's why we need to count these pages as they have out-links to
every single page in graph. But this will **significantly** increase size of in-links for every page.

It's pretty obvious that addtitional PR from dangling pages, is the same for all pages. So we can evaluate
it only once, and then add it to every page.

```
PR2[i] = PR1[i] + Dangling_pages_PR / page_cnt
```

### Damping factor and random jumps

In original page from Larry Page and Sergey Brin, they use some factor called damping factor to model
situation when user just stop web-surfing and go to random page. We will call this situation random jump.
To deal with this, we need damping factor(near 0.85), to say that with probability 0.85 user will continue
web-surfing, otherwise go to random page with probability of 0.15.

```
PR[i] = PR2[i] * damping_factor + (1 - damping_factor) / page_cnt
```

## How to use

To run the binaries correctly you should supply some command line parameters. Every binary has it's
built-in help(to call just add `-help`).

Let's run `omp` binary on some dataset:

```(shell)
$ omp
    --dataset /path/to/dataset
    --output /path/to/output
```

### Toolset

There are also `generate_dataset` tool provided to create random graph. It's able to create pretty huge
graphs, because it's using only O(n) amount of RAM memory. This is how to use it:

```(shell)
$ generate_dataset \
    --output /path/to/output/ \
    --page_cnt 1000000 \
    --name dataset-name
```

### Docker

It's super easy to distribute binaries via Docker. Here is an example how to run
PageRank in Docker container:

```(shell)
$ docker run --rm -it -v /path/to/data/on/host:/data lionell/parallel-pagerank bash
```

## Output example

Here is output from running `omp` version of PageRank with `1m_10k` dataset on my laptop:

```
$ bazel run :omp -- --dataset $(pwd)/data/generated/1m_10k
INFO: Found 1 target...
Target //:omp up-to-date:
  bazel-bin/omp
INFO: Elapsed time: 0.137s, Critical Path: 0.00s

INFO: Running command line: bazel-bin/omp --dataset data/generated/1m_10k
I0314 15:38:47.344501 28376 benchmark.h:43] Reading pages took 5.072125s cpu, 5.072000s wall.
I0314 15:38:56.033025 28376 omp.cc:72] Error 0.4260529474 at step 1
I0314 15:39:04.574093 28376 omp.cc:72] Error 0.0140949892 at step 2
I0314 15:39:13.116282 28376 omp.cc:72] Error 0.0005367169 at step 3
I0314 15:39:21.657510 28376 omp.cc:72] Error 0.0000204198 at step 4
I0314 15:39:30.199393 28376 omp.cc:72] Error 0.0000007763 at step 5
I0314 15:39:38.791136 28376 omp.cc:72] Error 0.0000000295 at step 6
I0314 15:39:38.792577 28376 benchmark.h:43] PageRank took 158.656886s cpu, 51.434000s wall.
```

## Building from source

[Bazel](https://bazel.build) is used as a main build system. To build the whole project just run `bazel build :all`.

**NOTE!** You need to use Bazel version that **supports protobufs**. As for now bazel-0.4.4 is the latest
release and there is no support for protobufs. Use version based on
[2046bb4](https://github.com/bazelbuild/bazel/commit/2046bb480075a8f412cb51882e64e31324fc57de) commit.

## Runtime dependencies

* [OpenMP](http://www.openmp.org)
* [OpenMPI](https://www.open-mpi.org) or [MPICH](https://www.mpich.org)
* [Protocol Buffers](https://developers.google.com/protocol-buffers) (serialization)
* [gflags](https://gflags.github.io/gflags) (cli flags)
* [glog](https://github.com/google/glog) (logging)

## License

Copyright © 2017 Ruslan Sakevych

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
