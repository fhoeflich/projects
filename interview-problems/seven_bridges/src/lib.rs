#![allow(unused)]

use std::collections::{HashMap, VecDeque};
use std::io::Error;

#[derive(Clone, Debug, PartialEq)]
pub struct NodeNotInGraph; //custom error type if node is not found in graph

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

    pub fn find_untraversed_neighbor(&mut self, from: &str) -> Result<String, NoUntraversedEdge> {
        let mut edges = Vec::new();

        //
        // Find an untraversed edge from `from' to any neighbor and return
        // that node name.
        // If no such edge exists, return NoUntraversedEdge.
        //
        println!("find_untraversed_neighbor({}): entered", from);
        match self.adjacency_matrix().get(from).clone() {
            Some(edges) => {
                // Note: using .get() avoids a "no entry found for key"
                //			panic if no such edge exists.
                println!("Found candidate edge list {:?}", edges);
            }
            None => {
                return Err(NoUntraversedEdge);
            }
        }
        // println!("edges is: {:?}", edges);

        // fn check_inventory(items: Vec<Item>, product: String) -> Vec<Item> {
        //    items.into_iter().filter(|i| i.name == product).collect()
        //}

        match edges.into_iter().find(|e: &Edge| !e.traversed) {
            Some(edge) => Ok(edge.to),
            None => Err(NoUntraversedEdge),
        }
    }

    fn populate(&mut self) {
        //
        // Populate the Konigsberg graph:
        // 1. Add the seven bridge edges (A,B), (B,C) etc.
        //    The first bridge from A->B is (A,B,1) and the second is (A,B,2).
        //    All edges are undirected/bidirectional.
        //
        self.add_edge("A", "B", 1, false); // all from A
        self.add_edge("A", "B", 2, false);
        self.add_edge("A", "D", 1, false);

        self.add_edge("B", "A", 1, false); // all from B
        self.add_edge("B", "A", 2, false);
        self.add_edge("B", "C", 1, false);
        self.add_edge("B", "C", 2, false);
        self.add_edge("B", "D", 1, false);

        self.add_edge("C", "B", 1, false); // all from C
        self.add_edge("C", "B", 2, false);
        self.add_edge("C", "D", 1, false);

        self.add_edge("D", "A", 1, false); // all from D
        self.add_edge("D", "B", 1, false);
        self.add_edge("D", "C", 1, false);
    }

    fn display(&mut self) {
        // Print out a simple ASCII A->B version of the graph.
        for (node, edgevec) in self.adjacency_matrix() {
            println!("node is {node}, edgevec is {edgevec:?}\n");
            for edge in edgevec.iter() {
                // println!("edge is {edge:?}");
                print!("{} -> {}({})  ", edge.from, edge.to, edge.id);
            }
            // println!("");
        }
    }

    fn reset(&mut self) {
        //
        // Reset all edges of the graph to not traversed.
        //
        for (node, edgevec) in self.adjacency_matrix() {
            // println!("node is {node}, edgevec is {edgevec:?}\n");
            for mut edge in edgevec.iter_mut() {
                println!("Setting traversed to false for edge {edge:?}");
                edge.traversed = false;
            }
            // println!("");
        }
    }

    // pub fn traverse_all(&mut self, from: &str)
    //
    // Traverse all paths starting from node `from'.
    // XXX: I suppose the return value should be the number of
    //		traversals completed, i.e. you get back to `from'.
    //
    // 	let mut ntraversed = 0;

    //   for (node, mut edgevec) in self.adjacency_matrix().clone() {
    //     for starter_edge in edgevec.iter_mut() {
    //           let from: String = starter_edge.from.clone();
    //           match from {
    //               ref val if *val == "A".to_string() => {
    //       			// for edge in self.neighbors(&from).iter_mut() {
    //       			for edge in edgevec.iter_mut() {
    // 						if edge.traversed == true {
    // 							continue;
    // 						}
    // 						edge.traversed = true;
    // 						ntraversed += 1;
    // 						println!("edge.from ======> {:?}",
    // 									edge.from);
    // 						println!("edge.to ======> {:?}",
    // 									edge.to);
    // 						println!("edge.id ======> {:?}",
    // 									edge.id);
    // println!("edge.traversed ======> {:?}",
    // 			edge.traversed);
    // 					}

    // 					if (ntraversed == 0) {
    // all edges starting at `from' have been traversed.
    // 						println!("all edges from node {} have been traversed",
    // 									from);
    // 					}
    //              }
    //               ref val if *val == "B".to_string() => {
    // println!("=======> {:?}", self.neighbors(&from).unwrap());
    // 			}
    //                 ref val if *val == "C".to_string() => {
    // println!("=======> {:?}", self.neighbors(&from).unwrap());
    // 				}
    //                 ref val if *val == "D".to_string() => {
    // println!("=======> {:?}", self.neighbors(&from).unwrap());
    // 				}
    //                 _ => {
    // 					panic!("Node not found: {:?}", from);
    // 				}
    //            	}
    // 			println!("");
    //         }
    //     }
    // }

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

        // let mut found_edge = Edge {
        //     from: "".to_string(),
        //     to: "".to_string(),
        //     id: 0,
        //     traversed: false,
        // };

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
                // found_edge = edge.clone();

                // XXX: add a trait so add_edge() can accept one Edge arg
                //		instead of four individual Edge field args.
                // queue.push_back(&edge.from, &edge.to, edge.id, edge.traversed);
                queue.push_back(edge.clone());

                if mru_edge.from != "" {
                    println!(
                        "MRU edge is {:?}, current edge is {:?}, != is {}",
                        mru_edge,
                        edge,
                        edge.from != mru_edge.to
                    );
                    if edge.from != mru_edge.to {
                        continue;
                    }
                }
                mru_edge = edge.clone();

                println!("Done with {:?}", edge);
            }
        }

        println!("Done all edges.");

        //
        // If edge.to == original_edge.from and all edges have been traversed,
        // set traversable to true.  Otherwise, we failed and it should be
        // set to false.
        //
        println!(
            "original_node is {}, mru_edge.to is {:?}",
            original_node, mru_edge.to
        );

        if mru_edge.to == original_node {
            traversable = true;
        } else {
            traversable = false;
        }

        traversable
    }

    pub fn neighbors(&mut self, from: &str) -> Result<&Vec<Edge>, NodeNotInGraph> {
        match self.adjacency_matrix().get(from) {
            Some(edges) => Ok(edges),
            // or simply:
            //
            // self.adjacency_matrix()
            //	.get(from)
            //	.ok_or(NodeNotInGraph)
            //
            // for more brevity.
            None => Err(NodeNotInGraph),
        }
    }
}

impl Iterator for MultiGraph {
    type Item = Edge;
    fn next(&mut self) -> Option<Self::Item> {
        // if self.start >= self.end {
        //     return None;
        // }
        // let result = Some(self.start);
        let hm = self.adjacency_matrix();
        let result = hm.get_key_value("B");
        println!("result is {:?}", result);
        // self.start += 1;
        // result
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    //    #[test]
    //    fn test_neighbors() {
    //        let mut graph = MultiGraph::new();
    //
    //        graph.populate();
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

    //     graph.populate();
    //     graph.display();
    // }

    #[test]
    fn test_is_traversable() {
        let mut graph = MultiGraph::new();
        let mut queue = VecDeque::new();
        let mut traversable = false;

        //
        // Case 1.  A simple two-node graph which is traversable.
        //			A->B, B->A
        //
        graph.add_edge("A", "B", 1, false);
        graph.add_edge("B", "A", 1, false);
        // graph.find_untraversed_neighbor("A");
        // graph.find_untraversed_neighbor("B");

        assert!(graph.is_traversable(&mut queue));
        println!("Traversed queue is: {:?}", queue);

        //
        // Case 2.  A simple three-node graph which is NOT traversable.
        //			A->B, B->A, B->C
        //
        graph.reset();
        graph.add_edge("B", "C", 1, false);
        queue.clear();
        graph.find_untraversed_neighbor("A");
        graph.find_untraversed_neighbor("B");
        graph.find_untraversed_neighbor("C");

        assert!(graph.is_traversable(&mut queue));
        println!("Traversed queue is: {:?}", queue);

        //
        // Case 3.  A simple three-node graph which IS traversable.
        //			A->B, B->A, A->C, C->A, B->C, C->B
        //
        // graph.reset();
        // graph.add_edge("A", "C", 1, false);
        // graph.add_edge("C", "A", 1, false);
        // graph.add_edge("C", "B", 1, false);
        // assert!(!graph.is_traversable(&mut queue));

        //
        // Case 4.  The Seven Bridges of Konigsberg graph.
        //
        // graph.populate();
        // let traversable = graph.is_traversable(&mut queue);
        // if traversable {
        // 	println!("Graph is traversable. Traversed graph is: {:?}",
        // 				traversed);
        // } else {
        // 	println!("Graph is not traversable.");
        // }
    }

    // #[test]
    // fn test_konigsberg() {
    //     let mut graph = MultiGraph::new();

    //     graph.populate();
    // }
}
