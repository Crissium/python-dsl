# Description

A Python package to convert DSL into HTML, written in modern C++.

The part that parses DSL into a DOM tree comes from [GoldenDict-ng](https://github.com/xiaoyifang/goldendict-ng/blob/staged/src/dict/dsl_details.cc), and I have made a number of changes:
- Use 8-bit `std::string` throughout.
- Use standard library equivalents of Qt classes/functions.
- Use functional programming wherever appropriate.
- Some tags are ignored.
And some other adjustments.

The part that converts the DOM tree into HTML is written by me, under influence from both [pyglossary](https://github.com/ilius/pyglossary) and GoldenDict-ng. Any bug here is mine :)

# Installation

```bash
pip install dsl2html
```

# Building

```bash
python3 setup.py build
```

Needless to say, you should have the development package of Python installed. Nothing other than `Python.h` is needed. Your compiler should support C++11, though.

# Usage

```python
>>> import dsl
>>> dsl.to_html(''' [m0][b]com·mu·ta·tor[/b] [p]7[/p] {{id=000008943}} [c rosybrown]\[[/c][c darkslategray][b]commutator[/b][/c] [c darkslategray][b]commutators[/b][/c][c rosybrown]\][/c] [p]BrE[/p] [c darkgray] [/c][c darkcyan]\[ˈkɒmjuteɪtə(r)\][/c] [s]z_commutator__gb_1.wav[/s] [p]NAmE[/p] [c darkgray] [/c][c darkcyan]\[ˈkɑːmjuteɪtər\][/c] [s]z_commutator__us_1.wav[/s] [c orange] noun[/c] [c darkgray] ([/c][c green]physics[/c][c darkgray])[/c]
...  [m1][c darkmagenta][b]1.[/b][/c] {{d}}a device that connects a motor to the electricity supply{{/d}}
...  [m1][c darkmagenta][b]2.[/b][/c] {{d}}a device for changing the direction in which electricity flows{{/d}}''', 'test')
(' <div style="margin-left: 0px;"><b>com·mu·ta·tor</b> <i><font color="green">7</font></i>  <span style="color: rosybrown;">[</span><span style="color: darkslategray;"><b>commutator</b></span> <span style="color: darkslategray;"><b>commutators</b></span><span style="color: rosybrown;">]</span> <i><font color="green">BrE</font></i> <span style="color: darkgray;"> </span><span style="color: darkcyan;">[ˈkɒmjuteɪtə(r)]</span> <audio controls autoplay src="/api/cache/test/z_commutator__gb_1.wav">z_commutator__gb_1.wav</audio> <i><font color="green">NAmE</font></i> <span style="color: darkgray;"> </span><span style="color: darkcyan;">[ˈkɑːmjuteɪtər]</span> <audio controls src="/api/cache/test/z_commutator__us_1.wav">z_commutator__us_1.wav</audio> <span style="color: orange;"> noun</span> <span style="color: darkgray;"> (</span><span style="color: green;">physics</span><span style="color: darkgray;">)</span> </div><div style="margin-left: 9px;"><span style="color: darkmagenta;"><b>1.</b></span> a device that connects a motor to the electricity supply </div><div style="margin-left: 9px;"><span style="color: darkmagenta;"><b>2.</b></span> a device for changing the direction in which electricity flows</div>', ['z_commutator__gb_1.wav', 'z_commutator__us_1.wav'])
```

The module has only one function: `to_html`, which takes two arguments: the DSL string and the dictionary name, and returns a tuple of two elements: the HTML string and a list of media file names.
