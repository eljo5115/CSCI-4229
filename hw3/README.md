<h1>Eli Jordan: Lorenz Attractor</h1>
<p>I compiled on MacOS with #ifdef for imports so it should work for Linux as well.</p>
<ol>
<li>Navigate into the directory <code>cd hw3</code></li>
<li>Run <code>make</code> this should make an executable called lorenz</li>
<li>James I've tested this makefile on a linux VM with no issues</li>
<li>Run <code>./hw3</code></li>
</ol>
<h3>Controls</h3>
<ul>
<li>Arrows to change view angle</li>
<li>ESC to exit</li>
<li>0 to reset view angle</li>
</ul>

<p>I spent about 3 hours working on this, partly on making the shapes and then using the parameters to generate the prefabs wherever I wanted. The forest and river are generated using a procedure to randomly disperse the trees and the river is a sine curve overlayed on the ground plane. The ground plane also uses a procedural height generation algorithm, although the differences in height are not very easy to see with the orthogonal camera.</p>