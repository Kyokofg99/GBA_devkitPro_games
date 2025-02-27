#!/usr/bin/python

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

import png,array
from sys import exit, argv
from getopt import getopt, GetoptError

def sampledown ((r,g,b)):
    return (r//8,g//8,b//8)

def BGRBitpack ((r,g,b)):
    return r|g<<5|b<<10

try :
    optlist, files = getopt(argv[1:], "o:")
    if optlist == [] :
        raise StandardError("")  
except GetoptError:
    print "usage: png2gba -o sprites.h image1.png image2.png"  
    exit(0)  
except StandardError:
    print "usage: png2gba -o sprites.h image1.png image2.png"  
    exit(0)  


outfile = optlist[0][1]

pixmaps = []
widths  = []
heights = []
for f in files :
    reader = png.Reader(f)
    w, h, pixels, metadata = reader.read() 
    widths.append(w)
    heights.append(h)


    pixmap = []
    for line in pixels:
        pixmap.append([])
        currentpixel = 0
        while currentpixel < len(line):
            #print line
            pixmap[-1].append((line[currentpixel],line[currentpixel+1],line[currentpixel+2]))
            currentpixel += 3

    pixmaps.append(pixmap)



#print pixmaps[0],"\n\n"
#print pixmaps[1],"\n\n"

print "Downsampling to 16 Bit.."

dspixmaps = []

for pixmap in pixmaps :
    dspixmap = []

    for line in pixmap:
        dsline = map(sampledown,line)
        dspixmap.append(dsline)

    dspixmaps.append(dspixmap)

#print dspixmaps[0],"\n\n"
#print dspixmaps[1],"\n\n"


print "Building Palette..."

palette = ["dummy"]
palpixmaps = []

for dspixmap in dspixmaps:
    
    palpixmap = []

    for dsline in dspixmap:
        palline = []
        for pixel in dsline:
            if pixel in palette:
                palline.append(palette.index(pixel))
                continue
            if len(palette) == 256:

                lowestdiff = 32
                bestpixel  = 0
                for num,cpixel in enumerate(palette) :
                    diff0 = abs(pixel[0] - cpixel[0])
                    diff1 = abs(pixel[1] - cpixel[1])
                    diff2 = abs(pixel[2] - cpixel[1])
                    diff  = (diff0+diff1+diff2)/3
                    
                    if diff < lowestdiff :
                        lowestdiff = diff
                        bestpixel  = num 

                pallline.append(bestpixel)

            else:
                palline.append(len(palette))
                palette.append(pixel)
        palpixmap.append(palline)

    palpixmaps.append(palpixmap)

palette[0] = (0,0,0)
for foo in palpixmaps[0]:
    print foo
#print palpixmaps[0],"\n\n"
#print palpixmaps[1],"\n\n"

outstr = "// Generated by png2gba. png2gba is Free Software and available\n// under the GNU Public Licence v3 (or higher) \n// For bugs and suggestions mail <fabian DOT vallon @@ gmx DOT de>\n\n"

while len(palette) < 256:
    palette.append((0,0,0))

brgpalette = map(BGRBitpack,palette)
strpalette = "const u16 mypalette[] = { " + ",".join(map(str,brgpalette)) + " };"
outstr += strpalette + "\n\n"

###############################################################

for num, palpixmap in enumerate(palpixmaps) :

    tiles = []
    htiles = widths[num]/8
    vtiles = heights[num]/8

    print htiles
    print vtiles

    for i in xrange(0,vtiles):
        for j in xrange(0,htiles):
            tile = []
            for tline in palpixmap[0+(8*i):8+(8*i)]:
                print tline, "::(",(8*j),",",8+8*j,")",tline[0+(8*j):8+(8*j)]
                tile.append(tline[0+(8*j):8+(8*j)])
                
            #print tile
            tiles.append(tile)

    strtilelist = []
    for tile in tiles:
        for tline in tile:
            print tline
            strtilelist.append( str(tline[0] | (tline[1] << 8)) )
            strtilelist.append( str(tline[2] | (tline[3] << 8)) )
            strtilelist.append( str(tline[4] | (tline[5] << 8)) )
            strtilelist.append( str(tline[6] | (tline[7] << 8)) )
        print ""

    varname = files[num].split("/")[-1].split("\\")[-1].split(".")[0]

    strtiles = "const u16 " +  varname + "[] = { " + ",".join(strtilelist) + " };"
    outstr += strtiles + "\n\n"

f = open(outfile,"w+")
f.write(outstr)
f.close()
