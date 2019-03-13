//
// Created by Yunming Zhang on 4/27/17.
//

#include <gtest.h>
#include "intrinsics.h"
#include "infra_gapbs/graph_verifier.h"
#include "infra_gapbs/ordered_processing.h"





class RuntimeLibTest : public ::testing::Test {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {

    }


    // Compares against simple serial implementation
    bool SSSPVerifier(const WGraph &g, NodeID source,
                      const pvector<WeightT> &dist_to_test) {



        // Serial Dijkstra implementation to get oracle distances
        pvector<WeightT> oracle_dist(g.num_nodes(), kDistInf);
        oracle_dist[source] = 0;
        typedef pair<WeightT, NodeID> WN;
        priority_queue<WN, vector<WN>, greater<WN>> mq;
        mq.push(make_pair(0, source));
        while (!mq.empty()) {
            WeightT td = mq.top().first;
            NodeID u = mq.top().second;
            mq.pop();
            if (td == oracle_dist[u]) {
                for (WNode wn : g.out_neigh(u)) {
                    if (td + wn.w < oracle_dist[wn.v]) {
                        oracle_dist[wn.v] = td + wn.w;
                        mq.push(make_pair(td + wn.w, wn.v));
                    }
                }
            }
        }
        // Report any mismatches
        bool all_ok = true;
        for (NodeID n : g.vertices()) {
            if (dist_to_test[n] != oracle_dist[n]) {
                cout << n << ": " << dist_to_test[n] << " != " << oracle_dist[n] << endl;
                all_ok = false;
            }
        }
        return all_ok;
    }



    // Compares against simple serial implementation
    bool PPSPVerifier(const WGraph &g, NodeID source, NodeID dest,
                      const pvector<WeightT> &dist_to_test) {
        // Serial Dijkstra implementation to get oracle distances
        pvector<WeightT> oracle_dist(g.num_nodes(), kDistInf);
        oracle_dist[source] = 0;
        typedef pair<WeightT, NodeID> WN;
        priority_queue<WN, vector<WN>, greater<WN>> mq;
        mq.push(make_pair(0, source));
        while (!mq.empty()) {
            WeightT td = mq.top().first;
            NodeID u = mq.top().second;
            mq.pop();
            if (td == oracle_dist[u]) {
                for (WNode wn : g.out_neigh(u)) {
                    if (td + wn.w < oracle_dist[wn.v]) {
                        oracle_dist[wn.v] = td + wn.w;
                        mq.push(make_pair(td + wn.w, wn.v));
                    }
                }
            }
        }


        // Report any mismatches
        //bool all_ok = true;
        //for (NodeID n : g.vertices()) {
        //if (dist_to_test[n] != oracle_dist[n]) {
        //    cout << n << ": " << dist_to_test[n] << " != " << oracle_dist[n] << endl;
        //    all_ok = false;
        //  }
        //}

        bool all_ok = false;
        if (dist_to_test[dest] == oracle_dist[dest]) all_ok = true;
        else {
            cout << "measured dist: " << dist_to_test[dest] << endl;
            cout << "oracle dist: " << oracle_dist[dest] << endl;
        }

        return all_ok;
    }


    const WeightT kDistInf = std::numeric_limits<WeightT>::max() / 2;

};


TEST_F(RuntimeLibTest, SimpleLoadGraphFromFileTest) {
    Graph g = builtin_loadEdgesFromFile("../../test/graphs/test.el");
    EXPECT_EQ (7 , g.num_edges());
}

TEST_F(RuntimeLibTest, SimpleLoadVerticesromEdges) {
    Graph g = builtin_loadEdgesFromFile("../../test/graphs/test.el");
    int num_vertices = builtin_getVertices(g);
    //max node id + 1, assumes the first node has id 0
    EXPECT_EQ (5 , num_vertices);
}

TEST_F(RuntimeLibTest, GetOutDegrees) {
    Graph g = builtin_loadEdgesFromFile("../../test/graphs/test.el");
    auto out_degrees = builtin_getOutDegrees(g);
    //max node id + 1, assumes the first node has id 0
    //TODO: fix this, we don't use vectors anymore
    //EXPECT_EQ (5 , out_degrees.size());
}

TEST_F(RuntimeLibTest, TimerTest) {
//    float start_time = getTime();
//    sleep(1);
//    float end_time = getTime();
    startTimer();
    sleep(1);
    float elapsed_time = stopTimer();
    std::cout << "elapsed_time: " << elapsed_time << std::endl;

    startTimer();
    sleep(2);
    elapsed_time = stopTimer();
    std::cout << "elapsed_time: " << elapsed_time << std::endl;
    EXPECT_EQ (5 , 5);
}

TEST_F(RuntimeLibTest, VertexSubsetSimpleTest) {
    bool test_flag = true;
    auto vertexSubset = new VertexSubset<int>(5, 0);
    for (int v = 0; v < 5; v = v+2){
        vertexSubset->addVertex(v);
    }

    for (int v = 1; v < 5; v = v+2){
        if (vertexSubset->contains(v))
            test_flag = false;
    }

    for (int v = 0; v < 5; v = v+2){
        if (!vertexSubset->contains(v))
            test_flag = false;
    }

    EXPECT_EQ(builtin_getVertexSetSize(vertexSubset), 3);


    delete vertexSubset;

    EXPECT_EQ(true, test_flag);

}


//test init of the eager priority queue based on GAPBS
TEST_F(RuntimeLibTest, EagerPriorityQueueInit) {

    WGraph g = builtin_loadWeightedEdgesFromFile("../../test/graphs/test.el");
    WeightT* dist_array = new WeightT[g.num_nodes()];
    for (int i = 0; i < g.num_nodes(); i++){
        dist_array[i] = kDistInf;
    }
    EagerPriorityQueue<WeightT> pq = EagerPriorityQueue<WeightT>(dist_array);

    EXPECT_EQ(pq.get_current_priority(), 0);
}

//test init of the buffered priority queue based on Julienne
TEST_F(RuntimeLibTest, BufferedPriorityQueueInit) {


    EXPECT_EQ(true, true);
}

// test compilation of the C++ version of SSSP using eager priority queue
TEST_F(RuntimeLibTest, SSSPOrderProcessingWithMergeTest){

    WGraph g = builtin_loadWeightedEdgesFromFile("../../test/graphs/test.el");
    WeightT* dist_array = new WeightT[g.num_nodes()];
    for (int i = 0; i < g.num_nodes(); i++){
        dist_array[i] = kDistInf;
    }

    NodeID source = 2;
    dist_array[source] = 0;

    EagerPriorityQueue<WeightT> pq = EagerPriorityQueue<WeightT>(dist_array);
    WeightT input_delta = 2;

    auto src_filter_func = [&](NodeID v)-> bool {
        return (dist_array[v] >= input_delta * static_cast<WeightT>(pq.get_current_priority()));
    };

    auto while_cond_func = [&]()->bool{
            return !pq.finished();
    };


    auto edge_update_func = [&](vector<vector<NodeID> >& local_bins, NodeID src, NodeID dst, WeightT wt) {
        WeightT old_dist = dist_array[dst];
        WeightT new_dist = dist_array[src] + wt;
        update_priority_min<WeightT>()(&pq, local_bins, dst, old_dist, new_dist);
    };

    OrderedProcessingOperatorWithMerge(&pq, g, dist_array, src_filter_func, while_cond_func, edge_update_func, 1000,  source);

    pvector<WeightT> dist = pvector<WeightT>(g.num_nodes());
    for (int i = 0; i < g.num_nodes(); i++){
        dist[i]= dist_array[i];
    }

    EXPECT_EQ(SSSPVerifier(g, source, dist), true);
}


// test compilation of the C++ version of SSSP using eager priority queue
TEST_F(RuntimeLibTest, SSSPOrderProcessingNoMergeTest){

    WGraph g = builtin_loadWeightedEdgesFromFile("../../test/graphs/test.el");
    WeightT* dist_array = new WeightT[g.num_nodes()];
    for (int i = 0; i < g.num_nodes(); i++){
        dist_array[i] = kDistInf;
    }

    NodeID source = 2;
    dist_array[source] = 0;

    EagerPriorityQueue<WeightT> pq = EagerPriorityQueue<WeightT>(dist_array);
    WeightT input_delta = 2;

    auto src_filter_func = [&](NodeID v)-> bool {
        return (dist_array[v] >= input_delta * static_cast<WeightT>(pq.get_current_priority()));
    };

    auto while_cond_func = [&]()->bool{
        return !pq.finished();
    };


    auto edge_update_func = [&](vector<vector<NodeID> >& local_bins, NodeID src, NodeID dst, WeightT wt) {
        WeightT old_dist = dist_array[dst];
        WeightT new_dist = dist_array[src] + wt;
        update_priority_min<WeightT>()(&pq, local_bins, dst, old_dist, new_dist);
    };

    OrderedProcessingOperatorNoMerge(&pq, g, dist_array, src_filter_func, while_cond_func, edge_update_func,  source);

    pvector<WeightT> dist = pvector<WeightT>(g.num_nodes());
    for (int i = 0; i < g.num_nodes(); i++){
        dist[i]= dist_array[i];
    }

    EXPECT_EQ(SSSPVerifier(g, source, dist), true);
}

// test compilation of the C++ version of PPSP using eager priority queue
TEST_F(RuntimeLibTest, PPSPOrderProcessingNoMergeTest){

    WGraph g = builtin_loadWeightedEdgesFromFile("../../test/graphs/test.el");
    WeightT* dist_array = new WeightT[g.num_nodes()];
    for (int i = 0; i < g.num_nodes(); i++){
        dist_array[i] = kDistInf;
    }

    NodeID source = 0;
    NodeID dest = 3;
    dist_array[source] = 0;

    EagerPriorityQueue<WeightT> pq = EagerPriorityQueue<WeightT>(dist_array);
    WeightT input_delta = 2;

    auto src_filter_func = [&](NodeID v)-> bool {
        return (dist_array[v] < dist_array[dest] && dist_array[v] >= input_delta * static_cast<WeightT>(pq.get_current_priority()));
    };

    auto while_cond_func = [&]()->bool{
            return dist_array[dest]/input_delta >=  pq.get_current_priority();
    };


    auto edge_update_func = [&](vector<vector<NodeID> >& local_bins, NodeID src, NodeID dst, WeightT wt) {
        WeightT old_dist = dist_array[dst];
        WeightT new_dist = dist_array[src] + wt;
        update_priority_min<WeightT>()(&pq, local_bins, dst, old_dist, new_dist);
    };

    OrderedProcessingOperatorNoMerge(&pq, g, dist_array, src_filter_func, while_cond_func, edge_update_func,  source);

    pvector<WeightT> dist = pvector<WeightT>(g.num_nodes());
    for (int i = 0; i < g.num_nodes(); i++){
        dist[i]= dist_array[i];
    }

    delete[] dist_array;

    EXPECT_EQ(PPSPVerifier(g, source, dest, dist), true);
}


TEST_F(RuntimeLibTest, PPSPOrderProcessingWithMergeTest){

    WGraph g = builtin_loadWeightedEdgesFromFile("../../test/graphs/test.el");
    WeightT* dist_array = new WeightT[g.num_nodes()];
    for (int i = 0; i < g.num_nodes(); i++){
        dist_array[i] = kDistInf;
    }

    NodeID source = 0;
    NodeID dest = 3;
    dist_array[source] = 0;

    EagerPriorityQueue<WeightT> pq = EagerPriorityQueue<WeightT>(dist_array);
    WeightT input_delta = 2;

    auto src_filter_func = [&](NodeID v)-> bool {
        return (dist_array[v] < dist_array[dest] && dist_array[v] >= input_delta * static_cast<WeightT>(pq.get_current_priority()));
    };

    auto while_cond_func = [&]()->bool{
        return dist_array[dest]/input_delta >=  pq.get_current_priority();
    };


    auto edge_update_func = [&](vector<vector<NodeID> >& local_bins, NodeID src, NodeID dst, WeightT wt) {
        WeightT old_dist = dist_array[dst];
        WeightT new_dist = dist_array[src] + wt;
        update_priority_min<WeightT>()(&pq, local_bins, dst, old_dist, new_dist);
    };

    OrderedProcessingOperatorWithMerge(&pq, g, dist_array, src_filter_func, while_cond_func, edge_update_func, 1000, source);

    pvector<WeightT> dist = pvector<WeightT>(g.num_nodes());
    for (int i = 0; i < g.num_nodes(); i++){
        dist[i]= dist_array[i];
    }

    delete[] dist_array;

    EXPECT_EQ(PPSPVerifier(g, source, dest, dist), true);
}

// test compilation of the C++ version of AStar using eager priority queue
TEST_F(RuntimeLibTest, AStar_test){

}

// test compilation of the C++ version of KCore using buffered priority queue
TEST_F(RuntimeLibTest, KCore_test){

}

