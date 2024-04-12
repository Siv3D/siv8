//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

////////////////////////////////////////////////////////////////
//
//	The MIT License
//
////////////////////////////////////////////////////////////////

# define MIT_LICENSE R"-(Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.)-"

////////////////////////////////////////////////////////////////
//
//	Apache License, Version 2.0
//
////////////////////////////////////////////////////////////////

# define APACHE_2_LICENSE R"-(Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.)-"

////////////////////////////////////////////////////////////////
//
//	The 3-Clause BSD License
//
////////////////////////////////////////////////////////////////

# define BSD_3_LICENSE R"-(Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are 
met:

    * Redistributions of source code must retain the above copyright 
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above 
      copyright notice, this list of conditions and the following 
      disclaimer in the documentation and/or other materials provided 
      with the distribution.
    * Neither the name of Google Inc. nor the names of its 
      contributors may be used to endorse or promote products derived 
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)-"

////////////////////////////////////////////////////////////////
//
//	The 2-Clause BSD License
//
////////////////////////////////////////////////////////////////

# define BSD_2_LICENSE R"-(Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this 
  list of conditions and the following disclaimer in the documentation and/or 
  other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)-"

////////////////////////////////////////////////////////////////
//
//	The zlib/libpng License
//
////////////////////////////////////////////////////////////////

# define ZLIB_LICENSE R"-(This software is provided ‘as-is’, without any express or implied 
warranty. In no event will the authors be held liable for any damages 
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it 
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not 
claim that you wrote the original software. If you use this software 
in a product, an acknowledgment in the product documentation would be 
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be 
misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.)-"


namespace s3d::detail
{
	static const LicenseInfo licenses[] = {
{
R"-(Siv3D Engine)-",
R"-(Copyright (c) 2008-2024 Ryo Suzuki
Copyright (c) 2016-2024 OpenSiv3D Project)-",
MIT_LICENSE
},

{
R"-(Abseil)-",
R"-(Copyright (c) 2017 The Abseil Authors.)-",
APACHE_2_LICENSE
},

{
R"-(CoreRT)-",
R"-(Copyright (c) .NET Foundation and Contributors)-",
MIT_LICENSE
},

{
R"-(cpu_features)-",
R"-(Copyright 2017 Google LLC
Copyright 2020 Intel Corporation)-",
APACHE_2_LICENSE
},

{
R"-(DirectXMath)-",
R"-(Copyright (c) Microsoft Corporation.)-",
MIT_LICENSE
},

{
R"-(doctest)-",
R"-(Copyright (c) 2016-2023 Viktor Kirilov)-",
MIT_LICENSE
},

{
R"-(double-conversion)-",
R"-(Copyright 2006-2011, the V8 project authors. All rights reserved.)-",
BSD_3_LICENSE
},

{
R"-(easyexif)-",
R"-(Copyright (c) 2010-2015 Mayank Lahiri
mlahiri@gmail.com
All rights reserved.)-",
BSD_2_LICENSE
},

{
R"-(fmt)-",
R"-(Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors)-",
MIT_LICENSE
},

{
R"-(GLFW)-",
R"-(Copyright (c) 2002-2006 Marcus Geelnard
Copyright (c) 2006-2019 Camilla Löwy)-",
ZLIB_LICENSE
},

{
R"-(levenshtein-sse)-",
R"-(Copyright (c) 2016 Anna Henningsen <sqrt@entless.org>)-",
MIT_LICENSE
},

{
R"-(libjpeg-turbo)-",
R"-()-",
R"-(This software is based in part on the work of the Independent JPEG Group.)-"
},

{
R"-(libjpeg-turbo)-",
R"-(Copyright (C)2009-2023 D. R. Commander. All Rights Reserved.
Copyright (C)2015 Viktor Szathmáry. All Rights Reserved.)-",
BSD_3_LICENSE
},

{
R"-(libpng)-",

R"-(Copyright (c) 1995-2024 The PNG Reference Library Authors.
Copyright (c) 2018-2024 Cosmin Truta.
Copyright (c) 2000-2002, 2004, 2006-2018 Glenn Randers-Pehrson.
Copyright (c) 1996-1997 Andreas Dilger.
Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc.)-",

R"-(PNG Reference Library License version 2
---------------------------------------

The software is supplied "as is", without warranty of any kind,
express or implied, including, without limitation, the warranties
of merchantability, fitness for a particular purpose, title, and
non-infringement.  In no even shall the Copyright owners, or
anyone distributing the software, be liable for any damages or
other liability, whether in contract, tort or otherwise, arising
from, out of, or in connection with the software, or the use or
other dealings in the software, even if advised of the possibility
of such damage.

Permission is hereby granted to use, copy, modify, and distribute
this software, or portions hereof, for any purpose, without fee,
subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software.  If you
    use this software in a product, an acknowledgment in the product
    documentation would be appreciated, but is not required.

 2. Altered source versions must be plainly marked as such, and must
    not be misrepresented as being the original software.

 3. This Copyright notice may not be removed or altered from any
    source or altered source distribution.

)-"
R"-(PNG Reference Library License version 1 (for libpng 0.5 through 1.6.35)
-----------------------------------------------------------------------

libpng versions 1.0.7, July 1, 2000 through 1.6.35, July 15, 2018 are
Copyright (c) 2000-2002, 2004, 2006-2018 Glenn Randers-Pehrson, are
derived from libpng-1.0.6, and are distributed according to the same
disclaimer and license as libpng-1.0.6 with the following individuals
added to the list of Contributing Authors:

    Simon-Pierre Cadieux
    Eric S. Raymond
    Mans Rullgard
    Cosmin Truta
    Gilles Vollant
    James Yu
    Mandar Sahastrabuddhe
    Google Inc.
    Vadim Barkov

and with the following additions to the disclaimer:

    There is no warranty against interference with your enjoyment of
    the library or against infringement.  There is no warranty that our
    efforts or the library will fulfill any of your particular purposes
    or needs.  This library is provided with all faults, and the entire
    risk of satisfactory quality, performance, accuracy, and effort is
    with the user.

Some files in the "contrib" directory and some configure-generated
files that are distributed with libpng have other copyright owners, and
are released under other open source licenses.

libpng versions 0.97, January 1998, through 1.0.6, March 20, 2000, are
Copyright (c) 1998-2000 Glenn Randers-Pehrson, are derived from
libpng-0.96, and are distributed according to the same disclaimer and
license as libpng-0.96, with the following individuals added to the
list of Contributing Authors:

    Tom Lane
    Glenn Randers-Pehrson
    Willem van Schaik

libpng versions 0.89, June 1996, through 0.96, May 1997, are
Copyright (c) 1996-1997 Andreas Dilger, are derived from libpng-0.88,
and are distributed according to the same disclaimer and license as
libpng-0.88, with the following individuals added to the list of
Contributing Authors:

    John Bowler
    Kevin Bracey
    Sam Bushell
    Magnus Holmgren
    Greg Roelofs
    Tom Tanner

Some files in the "scripts" directory have other copyright owners,
but are released under this license.

libpng versions 0.5, May 1995, through 0.88, January 1996, are
Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc.

For the purposes of this copyright and license, "Contributing Authors"
is defined as the following set of individuals:

    Andreas Dilger
    Dave Martindale
    Guy Eric Schalnat
    Paul Schmidt
    Tim Wegner

The PNG Reference Library is supplied "AS IS".  The Contributing
Authors and Group 42, Inc. disclaim all warranties, expressed or
implied, including, without limitation, the warranties of
merchantability and of fitness for any purpose.  The Contributing
Authors and Group 42, Inc. assume no liability for direct, indirect,
incidental, special, exemplary, or consequential damages, which may
result from the use of the PNG Reference Library, even if advised of
the possibility of such damage.

Permission is hereby granted to use, copy, modify, and distribute this
source code, or portions hereof, for any purpose, without fee, subject
to the following restrictions:

 1. The origin of this source code must not be misrepresented.

 2. Altered versions must be plainly marked as such and must not
    be misrepresented as being the original source.

 3. This Copyright notice may not be removed or altered from any
    source or altered source distribution.

The Contributing Authors and Group 42, Inc. specifically permit,
without fee, and encourage the use of this source code as a component
to supporting the PNG file format in commercial products.  If you use
this source code in a product, acknowledgment is not required but would
be appreciated.)-"
},

{
R"-(mimalloc)-",
R"-(Copyright (c) 2018-2021 Microsoft Corporation, Daan Leijen)-",
MIT_LICENSE
},

{
R"-(miniutf)-",
R"-(Copyright (c) 2013 Dropbox, Inc.)-",
MIT_LICENSE
},

{
R"-(nanobench)-",
R"-(Copyright (c) 2019-2023 Martin Leitner-Ankerl)-",
MIT_LICENSE
},

{
R"-(Oniguruma)-",
R"-(Copyright (c) 2002-2021  K.Kosako  <kkosako0@gmail.com>
All rights reserved.)-",
BSD_2_LICENSE
},

{
R"-(pcg-cpp)-",
R"-(Copyright (c) 2014-2017 Melissa O'Neill and PCG Project contributors)-",
MIT_LICENSE
},

{
R"-(simde)-",
R"-(Copyright (c) 2017 Evan Nemerson <evan@nemerson.com>)-",
MIT_LICENSE
},

{
R"-(simdutf)-",
R"-(Copyright 2021 The simdutf authors)-",
MIT_LICENSE
},

{
R"-(stduuid)-",
R"-()-",
MIT_LICENSE
},

{
R"-(The Parallel Hashmap)-",
R"-(Copyright (c) 2019, Gregory Popovitch - greg7mdp@gmail.com)-",
APACHE_2_LICENSE
},

{
R"-(tinycolormap)-",
R"-(Copyright (c) 2018 Yuki Koyama)-",
MIT_LICENSE
},

{
R"-(The Parallel Hashmap)-",
R"-(Copyright 2018 The Abseil Authors.)-",
APACHE_2_LICENSE
},

{
R"-(Xoshiro-cpp)-",
R"-(Copyright (c) 2020 Ryo Suzuki <reputeless@gmail.com>)-",
MIT_LICENSE
},

{
R"-(xxHash)-",
R"-(Copyright (c) 2012-2021 Yann Collet
All rights reserved.)-",
BSD_2_LICENSE
},

{
R"-(zlib)-",
R"-((C) 1995-2022 Jean-loup Gailly and Mark Adler)-",
ZLIB_LICENSE
},

{
R"-(Zstandard)-",
R"-(Copyright (c) Meta Platforms, Inc. and affiliates. All rights reserved.)-",
BSD_3_LICENSE
},
	};
}

# undef MIT_LICENSE
# undef APACHE_2_LICENSE
# undef BSD_3_LICENSE
# undef BSD_2_LICENSE
# undef ZLIB_LICENSE


/*

{
R"-()-",
R"-()-",
R"-()-"
},

*/