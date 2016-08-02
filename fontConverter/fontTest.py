#!/usr/bin/env python


import sys
import socket
import time
import ttfquery
import ttfquery.glyphquery
import ttfquery.glyph

from ttfquery._scriptregistry import registry

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect(("esp8266.local", 1337))


def printMetaData(metadata):
    print '    Specific Name:', metadata[2]
    print '    File:', metadata[0]
    print '    Modifiers:', metadata[1]
    print '    Family Name:', ", ".join(metadata[4])


usage = """metadata_query name

Will create a registry file font.cache if it doesn't
already exist, otherwise will just use the existing
cache.  See ttffiles.py for updating the cache.
"""
if sys.argv[1:]:
    name = " ".join(sys.argv[1:])
else:
    sys.stderr.write(usage)
    sys.exit(1)
fontNames = registry.matchName(name)
fontNames.sort()
for general in fontNames:
    general = fontNames[1]
    print 'Font:', general
    specifics = registry.fontMembers(general)
    specifics.sort()
    for specific in specifics:
        printMetaData(registry.metadata(registry.fontFile(specific)))

fontObject = ttfquery.describe.openFont("/Library/Fonts/Luminari.ttf")
print "Font Object", fontObject

fontHeight = ttfquery.glyphquery.lineHeight(fontObject)
fontWidth = ttfquery.glyphquery.width(fontObject, "x")
print "W:", fontWidth, "H:", fontHeight

userScale = 4
scale = fontHeight / 500
charWidth = fontWidth / scale * 1.1
ystart = 800
xstart = 0

for letter in ['h', 'e', 'l', 'l', 'o']:
    glyph = ttfquery.glyph.Glyph(letter)
    #print "Glyph", glyph
    contours = glyph.calculateContours(fontObject)
    for contour in contours:
        points = ttfquery.glyph.decomposeOutline(contour, steps=20)
        for point in points:
            # cmd = point[0],"x",point[1],"y0z;"
            cmd = "%fx%fy0z;" % ((point[0] / scale + xstart) / userScale, (point[1] / scale + ystart) / userScale)
            print cmd
            #s.send(cmd)
            time.sleep(0.01)
        #s.send("1000z;")
        print "1000z;"
    xstart = xstart + charWidth
