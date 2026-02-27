#![allow(unused)]

use std::collections::HashMap;

#[derive(Debug)]
pub struct NodeNotInGraph; //custom error type if node is not found in graph

pub struct DirectedGraph {
    adjacency_matrix: HashMap<String, Vec<String>>,
}

pub struct UndirectedGraph {
    adjacency_matrix: HashMap<String, Vec<String>>,
}

#[derive(Debug)]
struct Edge {
	to: String,
	id: u32,			// 1 for first A->B edge, 2 for second etc.
	traversed: bool,
}

#[derive(Debug)]
pub struct MultiGraph {
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

    fn neighbors(&mut self, node: &str) -> Result<&Vec<String>, NodeNotInGraph> {
        match self.adjacency_matrix().get(node) {
            None => Err(NodeNotInGraph),
            Some(i) => Ok(i),
        }
    }
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
    fn new() -> MultiGraph {
        MultiGraph {
            adjacency_matrix: HashMap::new(),
        }
    }

	fn adjacency_matrix(&mut self) -> &mut HashMap<String, Vec<Edge>> {
		&mut self.adjacency_matrix
	}

    fn add_edge(&mut self, from: &str, to: &str, id: u32) {
		let edges = self.adjacency_matrix()
					.entry(from.to_string())
					.or_insert(Vec::new());

		edges.push(Edge {
				to: to.to_string(),
				id,
				traversed: false,
		});
    }

    fn neighbors(&mut self, from: &str) -> Result<&Vec<Edge>, NodeNotInGraph> {
        match self.adjacency_matrix()
			.get(from) {
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

	fn mark_traversed(&mut self, from: &str, to: &str, id: u32) {
	}
}

#[cfg(test)]
// mod test_undirected_graph {
mod tests {
    use super::*;

//     #[test]
//     fn test_neighbors() {
//         let mut graph = UndirectedGraph::new();
// 
//         graph.add_edge(("A", "B"));
//         graph.add_edge(("A", "B"));
// 
//         graph.add_edge(("B", "C"));
//         graph.add_edge(("B", "C"));
// 
//         graph.add_edge(("A", "D"));
//         graph.add_edge(("B", "D"));
//         graph.add_edge(("C", "D"));
// 
//         assert_eq!(
//             graph.neighbors("A").unwrap(),
//             &vec![
//                 (String::from("B")),
//                 (String::from("B")),
//                 (String::from("D")),
//             ]
//         );
// 
//         assert_eq!(
//             graph.neighbors("B").unwrap(),
//             &vec![
//                 (String::from("A")),
//                 (String::from("A")),
//                 (String::from("C")),
//                 (String::from("C")),
//                 (String::from("D")),
//             ]
//         );
// 
//         assert_eq!(
//             graph.neighbors("C").unwrap(),
//             &vec![
// 				(String::from("B")),
// 				(String::from("B")),
// 				(String::from("D")),
// 			]
//         );
// 
//         assert_eq!(
//             graph.neighbors("D").unwrap(),
//             &vec![
// 				(String::from("A")),
// 				(String::from("B")),
// 				(String::from("C")),
// 			]
//         );
//     }

    //     #[test]
    //     fn test_directed() {
    //         let mut graph = DirectedGraph::new();
    //
    //         graph.add_edge(("a", "b", 5));
    //         graph.add_edge(("b", "c", 10));
    //         graph.add_edge(("c", "a", 7));
    //         graph.add_edge(("b", "a", 5));
    //
    //         assert_eq!(graph.neighbors("a").unwrap(), &vec![(String::from("b"), 5)]);
    //         assert_eq!(
    //             graph.neighbors("b").unwrap(),
    //             &vec![(String::from("c"), 10), (String::from("a"), 5)]
    //         );
    //     }

    	#[test]
    	fn test_konigsberg() {
            let mut graph = MultiGraph::new();
    
    		//
    		// Add the seven bridges (A,B), (B,C) etc.
			// The first bridge from A->B is (A,B,1) and the second is (A,B,2).
    		//
            graph.add_edge("A", "B", 1);
            graph.add_edge("A", "B", 2);

            graph.add_edge("B", "C", 1);
            graph.add_edge("B", "C", 2);

            graph.add_edge("A", "D", 1);
            graph.add_edge("B", "D", 1);
            graph.add_edge("C", "D", 1);

            // assert_eq!(
            //     graph.neighbors("A").unwrap(),
            //     &vec![(String::from("B")), (String::from("C"))]
            // );
    	}
}
