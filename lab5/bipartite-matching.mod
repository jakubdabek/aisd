param n, integer, >= 2;
/* number of nodes */

set V1, default {1..n};
set V2, default {1..n};
/* sets of nodes */

set E, within V1 cross V2;
/* set of arcs */

param capacity{(i,j) in E}, binary;
/* marks existence of edges */

var x{(i,j) in E}, binary;
/* says whether a given edge is in the match */

s.t. oneout{i in V1}:
/* there is at most one edge coming out of each vertex in V1 */
   sum{(i,j) in E} x[i,j] <= 1;
   
s.t. onein{j in V2}:
/* there is at most one edge coming into each vertex in V2 */
   sum{(i,j) in E} x[i,j] <= 1;

maximize obj: sum{(i,j) in E} x[i,j];
/* objective is to maximize the number of edges in the match */

solve;

end;
