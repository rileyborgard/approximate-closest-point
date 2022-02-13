# Approximate Closest Point Queries

This is an implementation of a method to answer approximate closest-point queries using the Hilbert curve. The Hilbert curve gives an ordering of the points in a square so that points close in the ordering are also close in 2D space. I was curious how well it could do the reverse. That is, can we find the closest point in space using the Hilbert ordering? For example, we can order all the input points in Hilbert order, and to answer a query, we find where it falls in the order and only consider the closest points in that order.

Well, it can't do it perfectly. But maybe if we have a few different Hilbert orderings corresponding to different rotations/parameters, the closest point from all orderings will be a very good approximation. This is what I implemented here, and visualized it like a Voronoi diagram.

![ ](demo.png)
