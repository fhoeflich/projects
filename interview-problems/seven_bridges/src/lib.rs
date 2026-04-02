#![allow(unused)]

use log::{info, trace, warn};
use simple_logger::*;
use std::collections::{HashMap, HashSet, VecDeque};
use std::fmt;
use std::io::Error;

#[derive(Clone, Debug, PartialEq)]
pub struct NodeNotInGraph; // custom error type if node is not found in graph

#[derive(Clone, Debug, PartialEq)]
pub struct NoUntraversedEdge; // custom error type if no untraversed edge is found

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Edge {
    from: String,
    to: String,
    id: u32, // 1 for first A->B edge, 2 for second etc.
    pair_with_reverse: bool,
    traversed: bool,
}

#[derive(Debug)]
pub struct MultiGraph {
    total_edges: u8,
    adjacency_matrix: HashMap<String, Vec<Edge>>,
}

impl MultiGraph {
    pub fn new() -> MultiGraph {
        MultiGraph {
            total_edges: 7,
            adjacency_matrix: HashMap::new(),
        }
    }

    fn adjacency_matrix(&mut self) -> &mut HashMap<String, Vec<Edge>> {
        &mut self.adjacency_matrix
    }

    pub fn add_edge(&mut self, from: &str, to: &str, id: u32, traversed: bool) {
        let edges = self
            .adjacency_matrix()
            .entry(from.to_string())
            .or_insert(Vec::new());

        edges.push(Edge {
            from: from.to_string(),
            to: to.to_string(),
            id,
            pair_with_reverse: false,
            traversed,
        });
    }

    pub fn add_undirected_edge(&mut self, a: &str, b: &str, id: u32, traversed: bool) {
        //
        // Add both directions for one physical undirected edge.
        // Matching ids let us find the reverse counterpart.
        //
        let a_edges = self
            .adjacency_matrix()
            .entry(a.to_string())
            .or_insert(Vec::new());
        a_edges.push(Edge {
            from: a.to_string(),
            to: b.to_string(),
            id,
            pair_with_reverse: true,
            traversed,
        });

        let b_edges = self
            .adjacency_matrix()
            .entry(b.to_string())
            .or_insert(Vec::new());
        b_edges.push(Edge {
            from: b.to_string(),
            to: a.to_string(),
            id,
            pair_with_reverse: true,
            traversed,
        });
    }

    fn mark_edge_and_counterpart_traversed(
        &mut self,
        from: &str,
        to: &str,
    ) -> Result<(), NoUntraversedEdge> {
        //
        // Mark one directed edge as traversed, then mark its counterpart
        // (reverse direction with the same id) if present.
        //
        let mut selected_id: Option<u32> = None;
        let mut should_mark_counterpart = false;

        if let Some(edges) = self.adjacency_matrix.get_mut(from) {
            if let Some(edge) = edges.iter_mut().find(|e| e.to == to && !e.traversed) {
                edge.traversed = true;
                selected_id = Some(edge.id);
                should_mark_counterpart = edge.pair_with_reverse;
            }
        }

        let edge_id = match selected_id {
            Some(id) => id,
            None => return Err(NoUntraversedEdge),
        };

        if should_mark_counterpart {
            if let Some(edges) = self.adjacency_matrix.get_mut(to) {
                if let Some(counterpart) = edges
                    .iter_mut()
                    .find(|e| e.to == from && e.id == edge_id && !e.traversed)
                {
                    counterpart.traversed = true;
                }
            }
        }

        Ok(())
    }

    pub fn find_untraversed_neighbor(&mut self, from: String) -> Result<String, NoUntraversedEdge> {
        //
        // Find an untraversed edge from `from' to any neighbor and return
        // that node name.
        // If no such edge exists, return NoUntraversedEdge.
        //
        match self.adjacency_matrix().get(&from) {
            Some(edges) => {
                // Note: using .get() avoids a "no entry found for key"
                //			panic if no such edge exists.
                //info!("find_untraversed_neighbor: found candidate edge list {edges:?}");
                match edges.into_iter().find(|e: &&Edge| !e.traversed) {
                    Some(edge) => {
                        info!("find_untraversed_neighbor: found edge {edge:?}");
                        Ok(edge.to.clone())
                    }

                    None => Err(NoUntraversedEdge),
                }
            }

            None => {
                return Err(NoUntraversedEdge);
            }
        }
    }

    fn populate(&mut self, traversed: bool) {
        //
        // Populate the Konigsberg graph.
        // Keep bidirectional entries for neighbor lookup using paired edges,
        // so traversing one direction marks its reverse counterpart too.
        //
        self.add_undirected_edge("A", "B", 1, traversed);
        self.add_undirected_edge("A", "B", 2, traversed);
        self.add_undirected_edge("A", "D", 1, traversed);
        self.add_undirected_edge("B", "C", 1, traversed);
        self.add_undirected_edge("B", "C", 2, traversed);
        self.add_undirected_edge("B", "D", 1, traversed);
        self.add_undirected_edge("C", "D", 1, traversed);
    }

    fn display(&mut self) {
        // Print out a simple ASCII A->B version of the graph.
        for (node, edgevec) in self.adjacency_matrix() {
            info!("node is {node}, edgevec is {edgevec:?}\n");
            for edge in edgevec.iter() {
                print!("{} -> {}({})  ", edge.from, edge.to, edge.id);
            }
            // info!("");
        }
    }

    fn reset(&mut self) {
        //
        // Reset all edges of the graph to not traversed.
        //
        for (node, edgevec) in self.adjacency_matrix() {
            // info!("reset: node is {node}, edgevec is {edgevec:?}\n");
            for mut edge in edgevec.iter_mut() {
                //info!("reset: setting traversed to false for edge {edge:?}");
                edge.traversed = false;
            }
            // info!("");
        }
    }

    fn clear(&mut self) {
        //
        // Empty the graph of all edges.
        //
        for (node, edgevec) in self.adjacency_matrix() {
            //info!("clear: clearing all edges in vec {edgevec:?}");
            edgevec.clear();
        }
    }

    fn all_traversed(&mut self) -> bool {
        //
        // Test that all edges of the graph are traversed (true)
        // otherwise at least one is untraversed (false).
        //
        for (node, edgevec) in self.adjacency_matrix() {
            //info!("all_traversed: node is {node}, edgevec is {edgevec:?}\n");
            for edge in edgevec.iter_mut() {
                //info!("all_traversed: processing edge {edge:?}");
                if edge.traversed == false {
                    info!("all_traversed: found untraversed edge {edge:?}");
                    return false;
                }
            }
            info!("");
        }

        info!("all_traversed: all edges traversed, returning true");
        return true;
    }

    fn from_to_edge(
        &mut self,
        from: &String,
        traversed: bool,
    ) -> Result<&mut Edge, NoUntraversedEdge> {
        //
        // Map a from node to an Edge with the indicated `traversed' value.
        //
        for (node, edgevec) in self.adjacency_matrix() {
            //info!("from_to_edge: node is {node}, edgevec is {edgevec:?}\n");
            for edge in edgevec.iter_mut() {
                //info!("from_to_edge: processing edge {edge:?}");
                if edge.from == *from && edge.traversed == traversed {
                    //info!("from_to_edge: found matching edge {edge:?}");
                    return Ok(edge);
                }
            }
            //info!("");
        }

        return Err(NoUntraversedEdge);
    }

    fn degree(&self, node: &str) -> usize {
        //
        // For an undirected graph (represented with bidirectional edges),
        // count the degree of a node by counting outgoing untraversed edges only.
        // (Since edges are bidirectional, counting out-edges gives the correct degree.)
        //
        match self.adjacency_matrix.get(node) {
            Some(edges) => edges.iter().filter(|e| !e.traversed).count(),
            None => 0,
        }
    }

    fn has_eulerian_path(&self) -> bool {
        //
        // Check if an Eulerian path exists in this undirected graph.
        // An undirected graph has an Eulerian path iff:
        // - All vertices with nonzero degree are connected, AND
        // - Either all vertices have even degree (Eulerian circuit), OR
        // - Exactly 2 vertices have odd degree (Eulerian path between them)
        //
        let mut odd_degree_count = 0;
        
        for node in self.adjacency_matrix.keys() {
            let deg = self.degree(node);
            if deg > 0 && deg % 2 == 1 {
                odd_degree_count += 1;
            }
        }
        
        // Valid if 0 odd vertices (circuit) or 2 odd vertices (path)
        odd_degree_count == 0 || odd_degree_count == 2
    }

    fn find_start_node(&self) -> String {
        //
        // Find the starting node for Hierholzer's algorithm.
        // If there's a node with odd degree, start from there.
        // Otherwise, start from any node with edges.
        //
        for (node, edges) in &self.adjacency_matrix {
            let deg = edges.iter().filter(|e| !e.traversed).count();
            if deg > 0 && deg % 2 == 1 {
                return node.clone();
            }
        }
        // If no odd degree vertex, find any vertex with edges
        for (node, edges) in &self.adjacency_matrix {
            let deg = edges.iter().filter(|e| !e.traversed).count();
            if deg > 0 {
                return node.clone();
            }
        }
        String::new()
    }

    pub fn dfs(&mut self, start_node: String, _current_node: String) -> bool {
        //
        // Hierholzer's Algorithm for finding Eulerian paths/circuits.
        // Returns true if an Eulerian path exists and all edges can be traversed
        // starting from start_node and returning to it.
        //
        // Algorithm:
        // 1. Check if an Eulerian path exists (degree conditions)
        // 2. Use a stack to build the path iteratively
        // 3. Mark edges as traversed throughout
        // 4. Return true if all edges are traversed at the end
        //

        info!("dfs (Hierholzer): start_node is {start_node}");

        let mut stack: Vec<String> = vec![start_node.clone()];
        let mut path: Vec<String> = Vec::new();
        let mut last_visited = start_node.clone();

        while !stack.is_empty() {
            let current = stack[stack.len() - 1].clone();
            info!("dfs: current node on stack is {current}");

            match self.find_untraversed_neighbor(current.clone()) {
                Ok(next_node) => {
                    // Mark the chosen edge and its counterpart (reverse edge with same id).
                    if self
                        .mark_edge_and_counterpart_traversed(&current, &next_node)
                        .is_ok()
                    {
                        info!("dfs: traversing edge {current} -> {next_node}");
                        last_visited = next_node.clone();
                        stack.push(next_node);
                    }
                }
                Err(NoUntraversedEdge) => {
                    // No more untraversed edges from current node
                    // Pop it from stack and add to path
                    let popped = stack.pop().unwrap();
                    path.push(popped.clone());
                    info!("dfs: no more edges from {popped}, added to path");
                }
            }
        }

        info!("dfs: path traversal complete, path length is {}", path.len());

        // Success requires consuming all edges and ending where we started.
        if self.all_traversed() && last_visited == start_node {
            info!("dfs: all edges traversed and returned to start, returning true");
            return true;
        }

        info!("dfs: traversal incomplete or did not return to start, returning false");
        return false;
    }

    pub fn is_traversable(&mut self, queue: &mut VecDeque<Edge>) -> bool {
        //
        // Try to traverse all paths starting from node `from'.
        // Return true if any complete traversals are made resulting
        // in arriving back at the starting node, otherwise return false.
        // If true, the traversed graph is passed back in `traversed'.
        //
        let mut traversable: bool;
        let mut final_edge = Edge {
            from: "".to_string(),
            to: "".to_string(),
            id: 0,
            pair_with_reverse: false,
            traversed: false,
        };
        let mut mru_edge = Edge {
            from: "".to_string(),
            to: "".to_string(),
            id: 0,
            pair_with_reverse: false,
            traversed: false,
        };
        let mut original_node: String = String::new();

        for (node, mut edgevec) in self.adjacency_matrix() {
            for mut edge in edgevec.iter_mut() {
                //
                // Save the original node we started from.  We should
                // finish up here in a successful traversal.
                //
                if original_node == "" {
                    original_node = edge.from.clone();
                }

                // Skip any previously traversed edge.
                if edge.traversed == true {
                    continue;
                }

                //
                // Current edge is now traversed.
                // Push it onto traversed edge queue so we can examine it
                // later to verify the path taken for full traversal, done
                // once-per-edge.
                //
                edge.traversed = true;

                // XXX: add a trait so add_edge() can accept one Edge arg
                //		instead of four individual Edge field args.
                // queue.push_back(&edge.from, &edge.to, edge.id, edge.traversed);
                queue.push_back(edge.clone());

                if mru_edge.from != "" {
                    //info!(
                    //   "MRU edge is {:?}, current edge is {:?}, != is {}",
                    //  mru_edge,
                    //    edge,
                    //    edge.from != mru_edge.to
                    //);
                    if edge.from != mru_edge.to {
                        continue;
                    }
                }
                mru_edge = edge.clone();

                //info!("Done with {:?}", edge);
            }
        }

        //info!("Done all edges.");

        //
        // If edge.to == original_edge.from and all edges have been traversed,
        // set traversable to true.  Otherwise, we failed and it should be
        // set to false.
        //
        //info!(
        //    "original_node is {}, mru_edge.to is {:?}",
        //    original_node, mru_edge.to
        //);

        if mru_edge.to == original_node {
            traversable = true;
        } else {
            traversable = false;
        }

        traversable
    }

    //    pub fn neighbors(&mut self, from: &str) -> Result<&Vec<Edge>, NodeNotInGraph> {
    //        match self.adjacency_matrix().get(from) {
    //            Some(edges) => Ok(edges),
    // or simply:
    //
    // self.adjacency_matrix()
    //	.get(from)
    //	.ok_or(NodeNotInGraph)
    //
    // for more brevity.
    //            None => Err(NodeNotInGraph),
    //        }
    //    }
}

#[cfg(test)]
mod tests {
    use super::*;

    //    #[test]
    //    fn test_neighbors() {
    //        let mut graph = MultiGraph::new();
    //
    //        graph.populate(false);
    //
    //        // A->B 1, A->B 2, A->D
    //        assert_eq!(
    //            graph.neighbors("A").unwrap(),
    //            &vec![
    //                (Edge {
    //                    from: String::from("A"),
    //                    to: String::from("B"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("A"),
    //                    to: String::from("B"),
    //                    id: 2,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("A"),
    //                    to: String::from("D"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //            ]
    //        );
    //
    //        // B->A 1, B->A 2, B->C 1, B->C 2, B->D
    //        assert_eq!(
    //            graph.neighbors("B").unwrap(),
    //            &vec![
    //                (Edge {
    //                    from: String::from("B"),
    //                    to: String::from("A"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("B"),
    //                    to: String::from("A"),
    //                    id: 2,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("B"),
    //                    to: String::from("C"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("B"),
    //                    to: String::from("C"),
    //                    id: 2,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("B"),
    //                    to: String::from("D"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //            ]
    //        );
    //
    //        // C->B 1, C->B 2, C->D 1
    //        assert_eq!(
    //            graph.neighbors("C").unwrap(),
    //            &vec![
    //                (Edge {
    //                    from: String::from("C"),
    //                    to: String::from("B"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("C"),
    //                    to: String::from("B"),
    //                    id: 2,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("C"),
    //                    to: String::from("D"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //            ]
    //        );
    //
    //        // D->A, D->B, D->C
    //        assert_eq!(
    //            graph.neighbors("D").unwrap(),
    //            &vec![
    //                (Edge {
    //                    from: String::from("D"),
    //                    to: String::from("A"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("D"),
    //                    to: String::from("B"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //                (Edge {
    //                    from: String::from("D"),
    //                    to: String::from("C"),
    //                    id: 1,
    //                    traversed: false
    //                }),
    //            ]
    //        );
    //
    //        //
    //        // Test a nonexistent node E to see if .neighbors() fails correctly.
    //        //
    //        assert_eq!(graph.neighbors("E"), Err(NodeNotInGraph));
    //    }

    // #[test]
    // fn test_display() {
    //     let mut graph = MultiGraph::new();

    //     graph.populate(false);
    //     graph.display();
    // }

    #[test]
    fn test_find_untraversed_neighbor() {
        let mut graph = MultiGraph::new();

        simple_logger::init().unwrap();

        //info!("-----------------------------------------");

        //
        // Case 1.  Two-node graph.
        //			Expect to always find the other A/B node; name it
        //			explicitly.
        //
        graph.add_edge("A", "B", 1, false);
        graph.add_edge("B", "A", 1, false);

        assert_eq!(
            graph.find_untraversed_neighbor("A".to_string()),
            Ok("B".to_string())
        );
        assert_eq!(
            graph.find_untraversed_neighbor("B".to_string()),
            Ok("A".to_string())
        );
        //info!("-----------------------------------------");

        //
        // Case 2.  Add a third node, also connected to B.
        //			Expect to find "some other" node; name it as a member of a
        //			small vec of allowable nodes.
        //
        graph.reset();
        graph.add_edge("B", "C", 1, false);
        graph.add_edge("C", "B", 1, false);

        let mut allowable_nodes = ["A", "C"];
        let mut result = graph.find_untraversed_neighbor("B".to_string()).unwrap();
        assert!(allowable_nodes.contains(&result.as_str()));

        allowable_nodes = ["A", "B"];
        result = graph.find_untraversed_neighbor("C".to_string()).unwrap();
        assert!(allowable_nodes.contains(&result.as_str()));
        //info!("-----------------------------------------");

        //
        // Case 3.  A fully-populated Konigsberg graph, all edges untraversed.
        //			Expect to find "some other" node; name it as a member of
        //			a potentially large HashSet.
        //
        graph.clear();
        graph.populate(false);

        let allowed: HashSet<&str> = ["B", "C", "D"].iter().cloned().collect();
        result = graph.find_untraversed_neighbor("A".to_string()).unwrap();
        assert!(allowed.contains(&result.as_str()));

        let allowed: HashSet<&str> = ["A", "C", "D"].iter().cloned().collect();
        result = graph.find_untraversed_neighbor("B".to_string()).unwrap();
        assert!(allowed.contains(&result.as_str()));

        let allowed: HashSet<&str> = ["A", "B", "D"].iter().cloned().collect();
        result = graph.find_untraversed_neighbor("C".to_string()).unwrap();
        assert!(allowed.contains(&result.as_str()));

        let allowed: HashSet<&str> = ["A", "B", "C"].iter().cloned().collect();
        result = graph.find_untraversed_neighbor("D".to_string()).unwrap();
        assert!(allowed.contains(&result.as_str()));
        //info!("-----------------------------------------");

        //
        // Case 4.  A fully-populated Konigsberg graph, all edges *traversed*.
        //			Expect to always get NoUntraversedEdge.
        //
        graph.clear();
        graph.populate(true);

        let mut err_result = graph
            .find_untraversed_neighbor("A".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        err_result = graph
            .find_untraversed_neighbor("B".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        err_result = graph
            .find_untraversed_neighbor("C".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        err_result = graph
            .find_untraversed_neighbor("D".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        //info!("-----------------------------------------");

        //
        // Case 5.  An empty graph.
        //			Expect to always get NoUntraversedEdge.
        //
        graph.clear();

        err_result = graph
            .find_untraversed_neighbor("A".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        err_result = graph
            .find_untraversed_neighbor("B".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        err_result = graph
            .find_untraversed_neighbor("C".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        err_result = graph
            .find_untraversed_neighbor("D".to_string())
            .unwrap_err();
        assert_eq!(err_result, NoUntraversedEdge);

        //info!("=========================================");
    }

    #[test]
    fn test_is_traversable() {
        let mut graph = MultiGraph::new();
        //let mut queue: std::collections::VecDeque<T> = VecDeque::new();
        let mut traversable = false;

        let _ = simple_logger::init();

        //
        // Case 1.  A simple two-node graph which is traversable.
        //			A->B, B->A
        //
        graph.add_edge("A", "B", 1, false);
        graph.add_edge("B", "A", 1, false);

        let allowable_roots: [String; 2] = ["A".to_string(), "B".to_string()];

        for start_node in allowable_roots {
            assert_eq!(graph.dfs(start_node.clone(), start_node.clone()), true);
            graph.reset();
        }

        // XXX: enable these when a stack or queue has been implemented to track the final path through the graph.
        //assert!(graph.is_traversable(&mut queue));
        //info!("Traversed queue is: {:?}", queue);

        //
        // Case 2.  A simple three-node graph which is NOT traversable.
        //			A->B, B->A, B->C
        //
        graph.reset();
        graph.add_edge("B", "C", 1, false);
        //queue.clear();

        let allowable_roots: [String; 3] = ["A".to_string(), "B".to_string(), "C".to_string()];

        for start_node in &allowable_roots {
            assert_eq!(graph.dfs(start_node.clone(), start_node.clone()), false);
            graph.reset();
        }

        // XXX: enable these when a stack or queue has been implemented to track the final path through the graph.
        //assert!(graph.is_traversable(&mut queue));
        //info!("Traversed queue is: {:?}", queue);

        //
        // Case 3.  A simple three-node graph which is traversable.
        //            A->B, B->C, C->A (or any cyclic permutation
        //            thereof, e.g. A->C, C->B, B->A).
        //
        graph.clear();
        graph.add_edge("A", "B", 1, false);
        graph.add_edge("B", "C", 1, false);
        graph.add_edge("C", "A", 1, false);

        for start_node in &allowable_roots {
            assert_eq!(graph.dfs(start_node.clone(), start_node.clone()), true);
            graph.reset();
        }

        // XXX: enable these when a stack or queue has been implemented to track the final path through the graph.
        // assert!(!graph.is_traversable(&mut queue));

        //
        // Case 4.  The Seven Bridges of Konigsberg graph.
        //
        graph.clear();
        graph.populate(false);

        let allowable_roots: [String; 4] = ["A".to_string(), "B".to_string(), "C".to_string(), "D".to_string()];

        for start_node in &allowable_roots {
            assert_eq!(graph.dfs(start_node.clone(), start_node.clone()), false);
            graph.reset();
        }

        // XXX: enable these when a stack or queue has been implemented to track the final path through the graph.
        //assert!(graph.is_traversable(&mut queue));
        //info!("Traversed queue is: {:?}", queue);
        // let traversable = graph.is_traversable(&mut queue);
        // if traversable {
        //     info!("Graph is traversable. Traversed graph is: {:?}", traversed);
        // } else {
        //     info!("Graph is not traversable.");
        // }
    }

    // #[test]
    // fn test_konigsberg() {
    //     let mut graph = MultiGraph::new();

    //     graph.populate(false);
    // }
}
