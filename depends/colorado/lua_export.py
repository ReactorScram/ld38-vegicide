__author__ = ["Troy"]
__version__ = "1.0"
__bpydoc__ = """This script exports a model to Lul Engine's Lua script format."""

bl_info = {
	"name": "Lul Model Format (.lua)",
	"description": "Exports models to a Lua script usable by Lul Engine",
	"author": "Troy",
	"version": (1, 0),
	"blender": (2, 5, 0),
	"api": 33427,
	"location": "File > Import-Export",
	"warning": "",
	"wiki_url": "",
	"tracker_url": "",
	"category": "Import-Export"}

import os
import bpy
import math

def edgeTuple (v0, v1):
	if v0 > v1:
		return (v1, v0)
	else:
		return (v0, v1)

def writeObjectMatrix (out, obj):
	out.write ('\t\tmatrix = {\n')
	order = (0, 1, 2, 3)
	
	for y in order:
		out.write ("\t\t\t")
		for x in order:
			out.write ('%f,' % (float (obj.matrix_local [y][x])))
		out.write ("\n")
	out.write ('\t\t},\n')

def writeObjectGroup (out, obj):
	out.write ('\t\tgroups = { ')
	
	for group in obj.users_group:
		out.write ('"%s", ' % group.name)
	
	
	out.write ('},\n')

def writeObjectParent (out, obj):
	# Check for MetaMesh parents
	if obj.parent:
		parentGroupName = None
		for parentGroup in obj.parent.users_group:
			if parentGroup.name.startswith ("LulMesh"):
				parentGroupName = parentGroup.name
		if parentGroupName != None:
			out.write ('\t\tparentName = "%s",\n' % (parentGroupName))
		else:
			# Maybe has a regular object parent
			out.write ('\t\tparentName = "%s",\n' % (obj.parent.name))
	else:
		out.write ('\t\tparentName = nil,\n')

def writeObjectAnimation (out, obj):
	if obj.animation_data != None:
		action = obj.animation_data.action
		out.write ('\t\tanimation = {\n')
		
		for fCurve in action.fcurves:
			out.write ('\t\t\t{\n')
			out.write ('\t\t\t\ttype = "%s",\n' % (fCurve.data_path))
			out.write ('\t\t\t\tarrayIndex = "%i",\n' % (fCurve.array_index))
			out.write ('\t\t\t\tkeyframes = {\n')
			
			for keyFrame in fCurve.keyframe_points:
				out.write ('\t\t\t\t\t{%f, %f},\n' % (keyFrame.co [0], keyFrame.co [1]))
				
			out.write ('\t\t\t\t}\n')
			
			out.write ('\t\t\t}\n')
		
		out.write ('\t\t},\n')
	else:
		out.write ('\t\tanimation = nil,\n')

def writeMeshVerts (out, mesh):
	out.write  ('\t\tverts = {\n')
	for i, vert in enumerate (mesh.vertices):
		x = vert.co [0]
		
		y = vert.co [1]
		z = vert.co [2]
		
		out.write ('\t\t\t{%f,%f,%f},\n' % (x, y, z))
	
	out.write ('\t\t},\n')

def writeMeshNormals (out, mesh):
	out.write ('\t\tnormals = {\n')
	for i, vert in enumerate (mesh.vertices):
		x = vert.normal [0]
		
		y = vert.normal [1]
		z = vert.normal [2]
		
		out.write ('\t\t\t{%f,%f,%f},\n' % (x, y, z))
	
	out.write ('\t\t},\n')

def writeMesh (out, mesh, name):
	# Update from ngons and quads and such to tessellated faces
	mesh.update (calc_tessface = True)
	
	out.write ('\t{\n')
	numVerts = len (mesh.vertices)
	numFaces = len (mesh.tessfaces)
	
	out.write ('\t\tname = "%s",\n' % (name))
	
	writeMeshVerts (out, mesh)
	writeMeshNormals (out, mesh)
	
	if False:
		print ("Trying to get textures for a mesh")
		print (mesh.name)
		print (mesh.uv_textures)
		
		out.write ('\t\ttexCoords = {\n')
		
		"""
		Dict of UV coords, one per vertex
		Note: This doesn't really work unless you split all edges, that's
		why I've disabled this whole block
		I'm also not clear on why I made this a dict, it should be a list?
		"""
		vertToUv = {}
		
		"""
		For every face, make a dict entry mapping each vertex to its UV coord
		Again, only works if the texture is seamless, which Blender doesn't enforce
		"""
		for faceIndex, face in enumerate (mesh.tessfaces):
			for uvIndex, uv in enumerate (uvLayer [faceIndex].uv):
				vertToUv [face.vertices [uvIndex]] = uv
		
		for uv in vertToUv:
			uvCoord = vertToUv [uv]
			
			out.write ("\t\t\t{%f, %f},\n" % (uvCoord [0], uvCoord [1]))
		
		out.write ('\t\t},\n')
	
	writeEdges = False
	
	if writeEdges:
		allEdges = {}
		
		for edge in mesh.edges:
			v = edge.vertices
			
			allEdges [edgeTuple (v[1], v[0])] = []
	
	out.write ('\t\tfaces = {\n')
	
	facesByMaterial = sorted (mesh.tessfaces, key = lambda x: x.material_index)
	
	lastMaterial = facesByMaterial [0].material_index
	
	materialStarts = {}
	materialStarts [lastMaterial] = 0
	
	index = 0
	for i, face in enumerate (facesByMaterial):
		v = face.vertices
		
		out.write ("\t\t\t{")
		for vertex in v:
			out.write ("%i," % vertex)
		
		out.write ("},\n")
		
		if face.material_index != lastMaterial:
			lastMaterial = face.material_index
			materialStarts [lastMaterial] = index
		
		index += len (v) * 3 - 6
	
	out.write ('\t\t},\n')
	
	# Find out if there's any texture coord data at all
	if len (mesh.tessface_uv_textures) > 0:
		out.write ("\t\tfaceUvCoordArray = {\n")
		for uvLayer in mesh.tessface_uv_textures:
			out.write ('\t\t\t{\n')
			out.write ("\t\t\tname = '%s',\n" % (uvLayer.name))
			out.write ("\t\t\tdata = {\n")
			
			for face in facesByMaterial:
				"""
				out.write ('\t\t\t\t{\n')
				for uvCoord in uvLayer.data [face.index].uv:
					out.write ('\t\t\t\t\t{%f, %f},\n' % (uvCoord [0], uvCoord [1]))
				
				out.write ('\t\t\t\t},\n')
				"""
				out.write ("""\t\t\t\t(function () 
				local t = {}
				local i = 1
				local function a (u, v) t [i] = {u, v} i = i + 1 end\n""")
				
				for uvCoord in uvLayer.data [face.index].uv:
					out.write ('\t\t\t\t\ta (%f, %f)\n' % (uvCoord [0], uvCoord [1]))
				
				out.write ("\t\t\t\treturn t end) (),\n")
			
			out.write ("\t\t\t},\n")
			out.write ('\t\t\t},\n')
		
		out.write ("\t\t},\n")
	else:
		out.write ('\t\ttexCoords = nil,\n')
		uvLayer = False
	
	out.write ("\t\tmaterials = {\n")
	
	for material_index, start in materialStarts.items ():
		if len (mesh.materials) > material_index:
			out.write ("\t\t\t{%i, '%s'},\n" % (start, mesh.materials [material_index].name))
	
	out.write ("\t\t},\n")
	
	out.write ('\t\t--numVerts = %i,\n\t\t--numFaces = %i,\n' % (numVerts, numFaces))
	
	out.write ('\t},\n')

def writeLuaFile (filename, optionDebugComments):
	# Exit edit mode, otherwise we can't export UVs
	if bpy.ops.object.mode_set.poll ():
		bpy.ops.object.mode_set (mode = "OBJECT")
	
	out = open (filename, "w")
	meshList = bpy.data.meshes
	
	meshObjects = []
	meshes = []
	meshObjectNames = {}
	
	ungroupedObjects = set (filter (lambda x: x.type == "MESH", bpy.data.objects))
	
	for group in bpy.data.groups:
		#out.write ("-- Exporting group %s\n" % group.name)
		
		rootObject = None
		
		objectSet = set (group.objects)
		
		groupMeshObjects = []
		
		for obj in group.objects:
			if obj.type == "MESH":
				if obj in ungroupedObjects:
					ungroupedObjects.remove (obj)
				
				if obj.parent in objectSet:
					# This object is a child within the group
					pass
				else:
					rootObject = obj
				
				groupMeshObjects.append (obj)
		
		if rootObject:
			# Select only this group
			
			duplicatedObjects = []
			duplicatedRoot = None
			
			for obj in groupMeshObjects:
				bpy.ops.object.select_all (action = "DESELECT")
				obj.select = True
				bpy.context.scene.objects.active = obj
				
				bpy.ops.object.duplicate ()
				bpy.ops.object.make_single_user (obdata = True)
				
				dup = bpy.context.scene.objects.active
				
				while len (dup.modifiers) >= 1:
					modifierName = dup.modifiers [0].name
					bpy.ops.object.modifier_apply (modifier = modifierName)
				
				duplicatedObjects.append (bpy.context.scene.objects.active)
				
				if obj == rootObject:
					duplicatedRoot = bpy.context.scene.objects.active
			
			for obj in duplicatedObjects:
				obj.select = True
			
			bpy.context.scene.objects.active = duplicatedRoot
			
			# Join the group
			bpy.ops.object.join ()
			
			bpy.ops.object.transform_apply (rotation = True, scale = True)
			
			joinedMesh = bpy.context.scene.objects.active.to_mesh (bpy.context.scene, False, "RENDER")
			meshes.append (joinedMesh)
			meshObjectNames [joinedMesh] = group.name
			bpy.ops.object.delete ()
	
	#out.write ("-- Exporting %i ungrouped objects\n" % len (ungroupedObjects))
	
	for obj in ungroupedObjects:
		# Select only this ungrouped object
		#out.write ("-- Selecting ungrouped object %s\n" % obj.name)
		bpy.context.scene.objects.active = obj
		bpy.ops.object.select_all (action = "DESELECT")
		obj.select = True
		
		bpy.ops.object.duplicate ()
		bpy.ops.object.make_single_user (obdata = True)
		
		dup = bpy.context.scene.objects.active
		
		while len (dup.modifiers) >= 1:
			modifierName = dup.modifiers [0].name
			bpy.ops.object.modifier_apply (modifier = modifierName)
		
		bpy.ops.object.transform_apply (location = True, rotation = True, scale = True)
		
		mesh = bpy.context.scene.objects.active.to_mesh (bpy.context.scene, False, "RENDER")
		
		meshObjects.append (mesh)
		meshObjectNames [mesh] = obj.name
		bpy.ops.object.delete ()
		
		#out.write ("-- Exporting ungrouped object %s\n" % obj.name)
	
	out.write ('local meshes = {\n')
	for mesh in meshes:
		name = meshObjectNames [mesh] or mesh.name
		writeMesh (out, mesh, name)
	
	for mesh in meshObjects:
		name = meshObjectNames [mesh] or mesh.name
		writeMesh (out, mesh, name)
	
	out.write ('}\n\n')
	"""
	out.write ('local objects = {\n')
	
	for i, obj in enumerate (meshObjects):
		out.write ('\t{\n')
		out.write ('\t\tname = "%s",\n' % (meshObjectNames [obj] or obj.name))
		writeObjectMatrix (out, obj)
		writeObjectGroup (out, obj)
		writeObjectParent (out, obj)
		if obj.type == "MESH":
			out.write ('\t\tmeshes = { %i },\n' % (i + 1))
		
		out.write ('\t},\n')
	
	out.write ('}\n\n')
	
	bpy.ops.object.select_all (action = "DESELECT")
	for obj in meshObjects:
		obj.select = True
	
	bpy.ops.object.delete ()
	"""
	out.write ('return {\n')
	out.write ('\tmeshes = meshes,\n')
	out.write ('\tobjects = objects,\n')
	out.write ('}\n\n')

# This bit is largely copied from Blender 2.56's DirectX export script

from bpy.props import *
from bpy_extras.io_utils import ExportHelper

class LulExporter (bpy.types.Operator, ExportHelper):
	'''Export to the Lul model format (.lua)'''
	
	bl_idname = "export_scene.lul_engine"
	bl_label = "Export Lul"
	
	filename_ext = ".lua"
	
	optionDebugComments = BoolProperty (
		name = "Insert debug comments",
		description = "Inserts comments so that individual vertex or face data can be found by index",
		default = False)
	
	def execute (self, context):
		filepath = self.filepath
		filepath = bpy.path.ensure_ext (filepath, self.filename_ext)
		
		writeLuaFile (self.filepath, self.optionDebugComments)
		return {"FINISHED"}

def menu_func_export (self, context):
	self.layout.operator (LulExporter.bl_idname, text = "Lul (.lua)")

def register ():
	bpy.utils.register_module (__name__)
	bpy.types.INFO_MT_file_export.append (menu_func_export)

def unregister ():
	bpy.utils.unregister_module (__name__)
	bpy.types.INFO_MT_file_export.remove (menu_func_export)

if __name__ == "__main__":
	unregister ()
	register ()
