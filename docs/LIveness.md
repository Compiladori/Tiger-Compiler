#### Liveness
A variable is live on an edge if there is a directed path from that
edge to a use of the variable that does not go through any def.

A variable is live-in at a node if it is live on any of the in-edges of that node.

A variable is live-out at a node if it is live on any of the out-edges of the node

.