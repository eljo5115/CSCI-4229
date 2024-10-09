<h1>Eli Jordan: Perspective Scene</h1>
<p>I compiled on MacOS with #ifdef for imports so it should work for Linux as well.</p>
<ol>
<li>Navigate into the directory <code>cd hw3</code></li>
<li>Run <code>make</code> this should make an executable called lorenz</li>
<li>Run <code>./hw4</code></li>
</ol>
<h3>Controls</h3>
<ul>
<li>w,a,s,d to move around the scene in first person</li>
<li>Arrows to change view angle</li>
<li>ESC to exit</li>
<li>0 to reset view angle</li>
</ul>

<p>This one took me a while. The orthogonal camera was already setup from hw3, and the top-down camera wasn't much more than moving around on the right path and clamping the view angle. However the first person camera gave me quite some grief. I understood so well conceptually what was <em>supposed</em> to be happening but in practice was clearly not going on. I finally found the solution was to change the near clipping plane. The movement is wonky since it doesn't exactly move based on the view direction, but that is coming next. </p>