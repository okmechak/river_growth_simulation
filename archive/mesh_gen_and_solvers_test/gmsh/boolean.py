# This reimplements gmsh/demos/boolean/boolean.geo in Python.
import sys
sys.path.append("/home/oleg/Programs/gmsh/lib")
import gmsh

model = gmsh.model
factory = model.occ

gmsh.initialize(sys.argv)

gmsh.option.setNumber("General.Terminal", 1)
gmsh.option.setNumber("Mesh.MshFileVersion", 2.2)
gmsh.option.setNumber("Mesh.RecombineAll", 1)

model.add("boolean")

# from http://en.wikipedia.org/wiki/Constructive_solid_geometry

gmsh.option.setNumber("Mesh.Algorithm", 6)
gmsh.option.setNumber("Mesh.CharacteristicLengthMin", 0.4)
gmsh.option.setNumber("Mesh.CharacteristicLengthMax", 0.4)

R = 1.4; Rs = R*.7; Rt = R*1.25

factory.addBox(-R, -R, -R, 2*R, 2*R, 2*R, 1)
factory.addSphere(0, 0, 0, Rt, 2)
factory.intersect([(3, 1)], [(3, 2)], 3)
factory.addCylinder(-2*R,0,0, 4*R,0,0, Rs, 4)
factory.addCylinder(0,-2*R,0, 0,4*R,0, Rs, 5)
factory.addCylinder(0,0,-2*R, 0,0,4*R, Rs, 6)
factory.fuse([(3, 4), (3, 5)], [(3, 6)], 7)
factory.cut([(3, 3)], [(3, 7)], 8)

factory.synchronize()

model.mesh.generate(3)
#model.mesh.refine()
#model.mesh.setOrder(2)
#model.mesh.partition(4)

# hide volume
model.setVisibility(model.getEntities(3),0)
# color all surfaces gold
model.setColor(model.getEntities(2),249,166,2)


gmsh.write("boolean.msh")
gmsh.fltk.run()


gmsh.finalize()