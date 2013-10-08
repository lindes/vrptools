vrptools - tools for Vision Research's Phantom cameras and their data
=====================================================================

What is this?
-------------

[Vision Research](http://www.visionresearch.com/) is a company that
makes some pretty amazing high-speed video cameras, which generally
have the word Phantom in their name.  Their products run the gamut
from doing [4K](http://en.wikipedia.org/wiki/4K_resolution) at 140
frames per second (fps) with the [Phantom
65](http://www.visionresearch.com/Products/High-Speed-Cameras/Phantom-65/),
all the way to an unbelievable 1,400,000 (yes, 1.4 *million*) fps
(though at a mere 128x8 *pixels*, no M, no K) on the [Phantom
v710](http://www.visionresearch.com/Products/High-Speed-Cameras/v710/)
-- with, of course, a whole breadth of in-between speeds and
resolutions, including some pretty impressive frame-rates at
[1080p](http://en.wikipedia.org/wiki/1080p) on some models.

This project, then, is an attempt to have a set of open-source
utilities for getting data out of the proprietary (though publicly
documented) "CINE" files.  There are, of course, a number of software
packages out there already that will read these files, including the
ones from Vision Research, other tools from
[Glue Tools](http://www.gluetools.com/products_phantom.html), and
others, but I was unable to find an open-source/free-software one,
so... here is this.

The cameras are fairly expensive, so I imagine their availability is
limited.  Still, there seem to be more and more people shooting with
them (as evidenced by a
[video search](http://www.google.com/search?q=phantom+high+speed&um=1&tbs=vid:1&source=og);
I got 4110 results as of 2011-02-23, and over 7 million as of
2013-10-08).  Add to this the fact that I happen to occassionaly have
had access to one, and I have some files here that I want to read, so,
here we are.

Status
------

These tools are currently in their *infancy*.

**Please keep your expectations low for this software, for now.**

That said, `cineinfo` is starting to provide some vaguely-useful
information about a CINE file, and `cine-extract` will now generate a
series of PPM files (in `cine-extract.d` by default; changeable with
`-d` option) from one, one per frame.

There's also a "magic" file (a la `/etc/magic`).  I haven't yet taken
the time to figure out how one would go about installing it on one's
system, but you can run `file -M magic foo.cine` and it will give some
info.  Probably still needs more content there, and probably also
needs to have some of it removed (or migrated to 'magic.verbose' or
something), as the output is really verbose at the moment.

Technical Docs:
---------------

This tooling is based on a document which, at the time of this
writing, was available for public download at:

[www.visionresearch.com/devzonedownloads/cine640.pdf](http://www.visionresearch.com/devzonedownloads/cine640.pdf)

Contributing:
-------------

If you'd like to contribute, please feel free to fork this project
from it's primary repository:
[github/lindes/vrptools](https://github.com/lindes/vrptools)

Then, create a [topic
branch](http://stackoverflow.com/questions/284514/what-is-a-git-topic-branch),
do your work, push it to your github fork, and send me a pull request.
I'll try to be prompt at taking a look.

You're also welcome to submit "Issues" via GitHub.

Or drop me a private message on there with any questions.

License information:
--------------------

Copyright 2011, 2013 by David Lindes.  All rights reserved.

The canonical source for this project is
[github.com/lindes/vrptools](https://github.com/lindes/vrptools).

This program is [free software](https://en.wikipedia.org/wiki/Free_software): you can redistribute it and/or modify
it under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Example commands:
-----------------

To get a bit of information about a file:

    ./cineinfo myfile.cine

To extract each frame as a PPM image:

     ./cine-extract -d myfile.ppms.d myfile.cine

TODO
----

* Create other output formats, including something suitable for
  streaming into [ffmpeg](http://www.ffmpeg.org/) or similar, for
  conversion to other formats.
