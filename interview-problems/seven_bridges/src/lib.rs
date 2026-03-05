#![allow(unused)]

use std::any::type_name_of_val;
use std::cmp::Ordering;
use std::collections::HashMap;

#[derive(Clone, Debug, PartialEq)]
pub struct NodeNotInGraph; //custom error type if node is not found in graph

pub struct DirectedGraph {
    adjacency_matrix: HashMap<String, Vec<String>>,
}

pub struct UndirectedGraph {
    adjacency_matrix: HashMap<String, Vec<String>>,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Edge {
    from: String,
    to: String,
    id: u32, // 1 for first A->B edge, 2 for second etc.
    traversed: bool,
}

impl Ord for Edge {
    fn cmp(&self, other: &Self) -> Ordering {
        other
            .to
            .to_string()
            .cmp(&self.to.to_string())
            .then_with(|| self.to.to_string().cmp(&other.to.to_string()))
    }
}

impl PartialOrd for Edge {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

#[derive(Debug)]
pub struct MultiGraph {
    total_edges: u8,
    adjacency_matrix: HashMap<String, Vec<Edge>>,
}

pub trait Graph {
    fn new() -> Self;
    fn adjacency_matrix(&mut self) -> &mut HashMap<String, Vec<String>>;

    fn add_node(&mut self, node: &str) -> bool {
        match self.adjacency_matrix().get(node) {
            None => {
                self.adjacency_matrix()
                    .insert((*node).to_string(), Vec::new());
                true
            }
            _ => false,
        }
    }

    fn add_edge(&mut self, edge: (&str, &str)) {
        self.add_node(edge.0);
        self.add_node(edge.1);

        self.adjacency_matrix()
            .entry(edge.0.to_string())
            .and_modify(|e| {
                e.push(edge.1.to_string());
            });
    }

    // fn neighbors(&mut self, node: &str) -> Result<&mut Vec<String>, NodeNotInGraph> {
    //    match self.adjacency_matrix().get(node) {
    //         None => Err(NodeNotInGraph),
    //         Some(mut i) => Ok(i),
    //     }
    // }
}

impl Graph for DirectedGraph {
    fn new() -> DirectedGraph {
        DirectedGraph {
            adjacency_matrix: HashMap::new(),
        }
    }

    fn adjacency_matrix(&mut self) -> &mut HashMap<String, Vec<String>> {
        &mut self.adjacency_matrix
    }
}

impl Graph for UndirectedGraph {
    fn new() -> UndirectedGraph {
        UndirectedGraph {
            adjacency_matrix: HashMap::new(),
        }
    }

    fn adjacency_matrix(&mut self) -> &mut HashMap<String, Vec<String>> {
        &mut self.adjacency_matrix
    }

    fn add_edge(&mut self, edge: (&str, &str)) {
        self.add_node(edge.0);
        self.add_node(edge.1);

        self.adjacency_matrix()
            .entry(edge.0.to_string())
            .and_modify(|e| {
                e.push(edge.1.to_string());
            });

        // Assignment lecture 156:  Add in connecting edge.1 to edge.0 so that
        // the graph is now undirected
        self.adjacency_matrix()
            .entry(edge.1.to_string())
            .and_modify(|e| {
                e.push(edge.0.to_string());
            });
    }
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
        for (node, edgevec) in self.adjacency_matrix() {
            println!("node is {node}, edgevec is {edgevec:?}\n");
            for edge in edgevec.iter() {
                // println!("edge is {edge:?}");
                print!("{} -> {}({})  ", edge.from, edge.to, edge.id);
            }
            println!("");
        }
    }

    // pub fn traverse_all(&mut self, from: &str) {
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

	pub fn traverse_all(&mut self) -> usize {
		//
		// Traverse all paths starting from node `from'.
		// Return the number of complete traversals made resulting in
		// arriving back at the starting node.
		//
		let mut ntraversed = 0;

		for (node, mut edgevec) in self.adjacency_matrix() {
			for mut edge in edgevec.iter_mut() {
				if edge.traversed == true {
					continue;
				}
				edge.traversed = true;
				ntraversed += 1;

				// println!("edge is {:?}", edge);
			}
		}

		ntraversed
	}

    pub fn neighbors(&mut self, from: &str) ->
				Result<&Vec<Edge>, NodeNotInGraph> {
        match self.adjacency_matrix().get(from) {
            None => Err(NodeNotInGraph),
            Some(edges) => Ok(edges),
            // or simply:
            //
            // self.adjacency_matrix()
            //	.get(from)
            //	.ok_or(NodeNotInGraph)
            //
            // for more brevity.
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

    #[test]
    fn test_neighbors() {
        let mut graph = MultiGraph::new();

        graph.populate();

        // A->B 1, A->B 2, A->D
        assert_eq!(
            graph.neighbors("A").unwrap(),
            &vec![
                (Edge {
                    from: String::from("A"),
                    to: String::from("B"),
                    id: 1,
                    traversed: false
                }),
                (Edge {
                    from: String::from("A"),
                    to: String::from("B"),
                    id: 2,
                    traversed: false
                }),
                (Edge {
                    from: String::from("A"),
                    to: String::from("D"),
                    id: 1,
                    traversed: false
                }),
            ]
        );

        // B->A 1, B->A 2, B->C 1, B->C 2, B->D
        assert_eq!(
            graph.neighbors("B").unwrap(),
            &vec![
                (Edge {
                    from: String::from("B"),
                    to: String::from("A"),
                    id: 1,
                    traversed: false
                }),
                (Edge {
                    from: String::from("B"),
                    to: String::from("A"),
                    id: 2,
                    traversed: false
                }),
                (Edge {
                    from: String::from("B"),
                    to: String::from("C"),
                    id: 1,
                    traversed: false
                }),
                (Edge {
                    from: String::from("B"),
                    to: String::from("C"),
                    id: 2,
                    traversed: false
                }),
                (Edge {
                    from: String::from("B"),
                    to: String::from("D"),
                    id: 1,
                    traversed: false
                }),
            ]
        );

        // C->B 1, C->B 2, C->D 1
        assert_eq!(
            graph.neighbors("C").unwrap(),
            &vec![
                (Edge {
                    from: String::from("C"),
                    to: String::from("B"),
                    id: 1,
                    traversed: false
                }),
                (Edge {
                    from: String::from("C"),
                    to: String::from("B"),
                    id: 2,
                    traversed: false
                }),
                (Edge {
                    from: String::from("C"),
                    to: String::from("D"),
                    id: 1,
                    traversed: false
                }),
            ]
        );

        // D->A, D->B, D->C
        assert_eq!(
            graph.neighbors("D").unwrap(),
            &vec![
                (Edge {
                    from: String::from("D"),
                    to: String::from("A"),
                    id: 1,
                    traversed: false
                }),
                (Edge {
                    from: String::from("D"),
                    to: String::from("B"),
                    id: 1,
                    traversed: false
                }),
                (Edge {
                    from: String::from("D"),
                    to: String::from("C"),
                    id: 1,
                    traversed: false
                }),
            ]
        );

        //
        // Test a nonexistent node E to see if .neighbors() fails correctly.
        //
        assert_eq!(graph.neighbors("E"), Err(NodeNotInGraph));
    }

    // #[test]
    // fn test_display() {
    //     let mut graph = MultiGraph::new();

    //     graph.populate();
    //     graph.display();
    // }

    #[test]
    fn test_traverse_all() {
        let mut graph = MultiGraph::new();
		let mut n = 0;

        graph.populate();
        let n = graph.traverse_all();
		println!("ntraversed at the end is: {}", n);
    }

    // #[test]
    // fn test_konigsberg() {
    //     let mut graph = MultiGraph::new();

    //     graph.populate();
    // }
}
