use std::collections::HashMap;

#[derive(Debug)]
pub struct NodeNotInGraph; //custom error type if node is not found in graph

pub struct DirectedGraph {
    adjacency_matrix: HashMap<String, Vec<String>>,
}

pub struct UndirectedGraph {
    adjacency_matrix: HashMap<String, Vec<String>>,
	// XXX: Pick up here.  How to add a bool named `traversed' to
	//		adjacency_matrix so that when looking up a node, we can
	//		tell whether or not an untraversed edge to that node
	//		still exists in our neighbors() list?
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

#[cfg(test)]
mod test_undirected_graph {
    use super::*;

    #[test]
    fn test_neighbors() {
        let mut graph = UndirectedGraph::new();

        graph.add_edge(("A", "B"));
        graph.add_edge(("A", "B"));

        graph.add_edge(("B", "C"));
        graph.add_edge(("B", "C"));

        graph.add_edge(("A", "D"));
        graph.add_edge(("B", "D"));
        graph.add_edge(("C", "D"));

        assert_eq!(
            graph.neighbors("A").unwrap(),
            &vec![
                (String::from("B")),
                (String::from("B")),
                (String::from("D")),
            ]
        );

        assert_eq!(
            graph.neighbors("B").unwrap(),
            &vec![
                (String::from("A")),
                (String::from("A")),
                (String::from("C")),
                (String::from("C")),
                (String::from("D")),
            ]
        );

        assert_eq!(
            graph.neighbors("C").unwrap(),
            &vec![
				(String::from("B")),
				(String::from("B")),
				(String::from("D")),
			]
        );

        assert_eq!(
            graph.neighbors("D").unwrap(),
            &vec![
				(String::from("A")),
				(String::from("B")),
				(String::from("C")),
			]
        );
    }

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
            let mut graph = UndirectedGraph::new();
    
    		//
    		// Add the seven bridges (A,B), (B,C) etc.
    		//
            graph.add_edge(("A", "B"));
            graph.add_edge(("A", "B"));

            graph.add_edge(("B", "C"));
            graph.add_edge(("B", "C"));

            graph.add_edge(("A", "D"));
            graph.add_edge(("B", "D"));
            graph.add_edge(("C", "D"));

            // assert_eq!(
            //     graph.neighbors("A").unwrap(),
            //     &vec![(String::from("B")), (String::from("C"))]
            // );
    	}
}
