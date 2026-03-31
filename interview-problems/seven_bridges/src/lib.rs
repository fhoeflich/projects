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
            traversed,
        });
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
        // Populate the Konigsberg graph:
        // 1. Add the seven bridge edges (A,B), (B,C) etc.
        //    The first bridge from A->B is (A,B,1) and the second is (A,B,2).
        //    All edges are undirected/bidirectional.
        //
        self.add_edge("A", "B", 1, traversed); // all from A
        self.add_edge("A", "B", 2, traversed);
        self.add_edge("A", "D", 1, traversed);

        self.add_edge("B", "A", 1, traversed); // all from B
        self.add_edge("B", "A", 2, traversed);
        self.add_edge("B", "C", 1, traversed);
        self.add_edge("B", "C", 2, traversed);
        self.add_edge("B", "D", 1, traversed);

        self.add_edge("C", "B", 1, traversed); // all from C
        self.add_edge("C", "B", 2, traversed);
        self.add_edge("C", "D", 1, traversed);

        self.add_edge("D", "A", 1, traversed); // all from D
        self.add_edge("D", "B", 1, traversed);
        self.add_edge("D", "C", 1, traversed);
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

    pub fn dfs(&mut self, start_node: String, current_node: String) -> bool {
        //
        // Perform a depth-first search of untraversed edges attached
        //	to `current_node'.
        //
        // Return true iff:
        //	1. At least one edge was traversed (i.e. at least one new node
        //		was visited);
        //	2. *All* available untraversed edges were tried; and
        //	3. The terminal node is `current_node', i.e. the path is a cycle.
        // otherwise return false.
        //
        info!("dfs: start_node is {start_node} current_node is {current_node}");

        if self.all_traversed() {
            if current_node == start_node {
                info!("dfs: got success on {current_node}!");
                return true;
            } else {
                // this path failed
                info!("dfs: got failure on {current_node} :-@(");
                return false;
            }
        }

        info!("dfs: for mut next_node loop current_node is {current_node}");
        for mut next_node in self
            .find_untraversed_neighbor(current_node.clone())
            .iter_mut()
        {
            info!("dfs: candidate next node is {next_node}");
            // Find the edge from current_node to next_node and mark it as traversed
            let edge: Result<&mut Edge, NoUntraversedEdge> =
                self.from_to_edge(&current_node, false);
            let mut new_current_node: String = "".to_string();

            match edge {
                Ok(edge) => {
                    info!("dfs: taking Ok match arm");
                    if edge.to == *next_node {
                        edge.traversed = true;
                        info!("dfs: after marking traversed, edge is {edge:?}");
                        new_current_node = edge.clone().to.to_string();
                        return self.dfs(start_node, new_current_node);
                    }
                }
                Err(NoUntraversedEdge) => {
                    info!("dfs: taking Err match arm");
                    return false;
                }
            }
        }

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
            traversed: false,
        };
        let mut mru_edge = Edge {
            from: "".to_string(),
            to: "".to_string(),
            id: 0,
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
        // graph.populate(false);
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
