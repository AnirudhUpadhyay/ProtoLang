<div align="center">
  <br>
  <h1>ProtoLang</h1>
  <strong>Exploring Compiler Internals</strong>
</div>
<br>
<p>Welcome to the ProtoLang codebase. ProtoLang is a LLVM-based prototype programming language that is built with learning in mind. I am excited to have you here. The only way to learn programming is to build, so dive in, break things, and learn as you go!</p>
<br>

<h2>About</h2>
<p><strong>ProtoLang</strong> is a lightweight, educational programming language built using C++ and the LLVM compiler framework. It's designed to explore the fundamentals of compiler development like tokenization, parsing, Abstract Syntax Tree construction, and generating Intermediate Representation through a modular and extensible architecture.</p>
<p>If you're here to understand tokenization, parsing, AST design, or LLVM IR generation, ProtoLang is a great place to start. While not intended for real-world development, it's a valuable tool for those curious about language design and compiler construction. </p>
<p>ProtoLang is a personal project inspired by LLVM’s Kaleidoscope tutorial, but has been extended and built from the ground up to improve understanding of how modern language tooling works.</p>
<blockquote>
  ⚠️ Note: This project is not intended for production use. It's a sandbox for learning compiler internals and LLVM infrastructure.
</blockquote>
<hr>

<h2>Features</h2>
<ul>
  <li>Basic, easy to modify tokenizer and parser</li>
  <li>Abstract Syntax Tree (AST) generation</li>
  <li>LLVM IR code generation</li>
  <li>Simple REPL loop for interactive input</li>
  <li>Modular structure with clean separation of concerns</li>
</ul>

<hr>

<h2>Getting Started</h2>

<h3>Prerequisites</h3>
<ul>
  <li>A C++ compiler (I only tested this with <code>clang++</code> yet)</li>
  <li><a href="https://llvm.org/" target="_blank">LLVM</a> version 3.7 or later (I built this with llvm 14.0.0)</li>
  <li>GNU Make</li>
</ul>

<h3>Build</h3>

<pre><code>git clone git@github.com:YourUsername/ProtoLang.git
cd ProtoLang
make
</code></pre>

<p>This compiles the project and creates a binary named <code>main</code>.</p>

<h3>Run</h3>

<pre><code>./main
</code></pre>

<p>This launches a simple REPL where you can define functions and evaluate expressions.</p>

<hr>

<h2>Example</h2>

<pre><code>def add(x y) x + y;
add(3, 5);
</code></pre>

<p>You can enter expressions and see LLVM IR generated and executed under the hood.</p>

<hr>

<h2>Project Structure</h2>

<pre><code>ProtoLang/
├── include/        # Header files (.h)
├── src/            # Source files (.cpp)
├── Makefile        # Build script
├── README.md       # Project documentation
└── main            # Built binary (created after running make)
</code></pre>

<h3>Key Files</h3>

<table>
  <thead>
    <tr>
      <th>File</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>main.cpp</code></td><td>Entry point and REPL loop</td></tr>
    <tr><td><code>token.cpp</code></td><td>Tokenization logic</td></tr>
    <tr><td><code>parser.cpp</code></td><td>Recursive descent parser</td></tr>
    <tr><td><code>ast.cpp</code></td><td>AST node definitions and LLVM IR code generation</td></tr>
    <tr><td><code>mainloop.cpp</code></td><td>Intermediate driver between components</td></tr>
  </tbody>
</table>

<hr>

<h2>Extending ProtoLang</h2>

<p>ProtoLang is designed to be a stepping stone for larger language projects. Potential extensions include:</p>
<ul>
  <li>Adding control flow (<code>if</code>, <code>for</code>, etc.)</li>
  <li>Adding variable bindings and scope management</li>
  <li>Introducing type checking and type inference</li>
  <li>Implementing error handling and diagnostics</li>
  <li>Adding JIT and optimisation support</li>
</ul>

<hr>

<h2>Development</h2>

<h3>Makefile</h3>

<p>The Makefile uses <code>llvm-config</code> to dynamically retrieve the correct flags:</p>

<pre><code>CPPFLAGS += $(shell llvm-config --cxxflags)
LDFLAGS += $(shell llvm-config --ldflags --libs)
</code></pre>

<p>You can modify the Makefile to add include directories, debug flags, or additional source files as the project grows.</p>

<hr>

<h2>License</h2>

<p>This project is open for learning and experimentation. You are free to fork and modify it under the terms of your own discretion. No formal license is applied.</p>

<hr>

<h2>Contributions</h2>

<p>While this started as a personal learning project, contributions are welcome. Feel free to open issues or pull requests if you're exploring similar topics or want to improve something.</p>

<hr>

<h2>Author</h2>

<p><strong>Anirudha Upadhyay</strong> – <em>CS student & low-level programming enthusiast</em></p>

<blockquote>
  Building to learn.
</blockquote>
