ZSCALE = 1.0

def writeHeader(file):
    file.write('<objects>\n\n')

def writeFooter(file):
    file.write('</objects>\n')

def writeTransform(file, orientation, zOffset, zScale):
    if orientation == 0 and zOffset == 0 and zScale == 1:
        return
    file.write('      <transform>')
    if orientation != 0: file.write('<rotatez angle="' + str(orientation * 90) + '" />')
    if zScale      != 1: file.write('<scale x="1" y="1" z="' + str(zScale) + '" />')
    if zOffset     != 0: file.write('<translate x="0" y="0" z="' + str(zOffset) + '" />')
    file.write('</transform>\n')

def writeSimpleMeshWithUvTransform(file, orientation, meshName, textureName, uvTransform):
    file.write('    <mesh file="' + meshName + '">\n')
    file.write('      <texture file="' + textureName + '" />\n')
    writeTransform(file, orientation, 0, ZSCALE)
    file.write(uvTransform)
    file.write('    </mesh>\n')
    
def writeSimpleMesh(file, orientation, meshName, textureName):
#   writeSimpleMeshWithUvTransform(file, orientation, meshName, textureName, '')
    file.write('    <mesh file="' + meshName + '"><texture file="' + textureName + '" /></mesh>\n')

def writeCharacterShadow(file):
    file.write('    <mesh file="shadow~0" transparent="true" depth-write="false"><texture file="shadow" /></mesh>\n')

def writeCharacterMeshSet(file, baseMeshName, textureList):
    count = 0
    for texture in textureList:
        writeSimpleMesh(file, 0, baseMeshName + "~" + str(count), texture)
        count = count + 1
        
def writeCharacterFrame(file, baseMeshName, textureList):
    file.write('  <object>\n')
    writeCharacterShadow(file)
    writeCharacterMeshSet(file, baseMeshName, textureList)
    file.write('  </object>\n')

def writeCharacterAnimation(file, character, name, frames, baseMeshName, textureList):
    file.write(' <list name="' + character + '.' + name + '">\n')
    for i in range(0, frames):
      writeCharacterFrame(file, baseMeshName + ("%(#)03d" % { '#' : i }), textureList)
    file.write(' </list>\n\n')

def writeSkeletonType(file, character, body, weapon):
    file = open(character + '-animation.xml', 'w')
    writeHeader(file)
    writeCharacterAnimation(file, character, 'Walk',   25, 'skelwalk_',  [ weapon, body, weapon ])
    writeCharacterAnimation(file, character, 'Idle',   11, 'skelidle_',  [ weapon, body, weapon ])
    writeCharacterAnimation(file, character, 'Run',    25, 'skelwalk_',  [ weapon, body, weapon ])
    writeCharacterAnimation(file, character, 'Attack', 19, 'skelfight_', [ body, weapon, weapon ])
    writeCharacterAnimation(file, character, 'Die',    16, 'skeldie_',   [ body, body, body, body, \
                                                                           body, body, body, body, \
                                                                           body, body, body, body, \
                                                                           body, body, body, body, \
                                                                           body, weapon ])
    writeFooter(file)
    file.close()    
  
# start writing
writeSkeletonType(file, 'Skeleton1', 'skeleton_03_small', 'skelweapon_01small')


