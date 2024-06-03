xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 11;
 12.75420;10.28868;0.82872;,
 0.00000;1.23072;73.68432;,
 12.75420;-10.34184;-4.11264;,
 -12.74748;-10.34184;-4.11264;,
 -12.74748;10.28868;0.82872;,
 0.00000;11.81208;-13.09440;,
 0.00000;-7.11972;-18.03600;,
 0.00000;14.55708;-2.24712;,
 0.00000;-12.95940;-5.09292;,
 0.00000;-12.95940;-5.09292;,
 0.00000;14.55708;-2.24712;;
 
 12;
 3;0,1,2;,
 3;3,1,4;,
 4;5,0,2,6;,
 4;3,4,5,6;,
 3;4,1,7;,
 3;1,0,7;,
 3;1,3,8;,
 3;2,1,8;,
 3;3,6,9;,
 3;6,2,9;,
 3;0,5,10;,
 3;5,4,10;;
 
 MeshMaterialList {
  1;
  12;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\MODEL\\dragonBoss000.png";
   }
  }
 }
 MeshNormals {
  17;
  0.960689;0.068308;-0.269092;,
  0.985063;-0.040109;0.167460;,
  0.741367;0.162864;-0.651038;,
  0.622072;0.604234;-0.497924;,
  -0.960713;0.068287;-0.269012;,
  -0.000081;0.999423;-0.033969;,
  -0.000054;-0.992581;-0.121587;,
  -0.985078;-0.040089;0.167374;,
  -0.622248;0.604157;-0.497796;,
  -0.741543;0.162816;-0.650850;,
  -0.324315;0.945403;-0.032132;,
  -0.000070;0.984946;0.172863;,
  0.324162;0.945455;-0.032136;,
  -0.000048;-0.984161;0.177277;,
  -0.208482;-0.970770;-0.118915;,
  0.208377;-0.970792;-0.118918;,
  -0.000055;-0.911518;-0.411260;;
  12;
  3;0,1,0;,
  3;4,7,4;,
  4;3,0,0,2;,
  4;4,4,8,9;,
  3;10,11,5;,
  3;11,12,5;,
  3;13,14,6;,
  3;15,13,6;,
  3;14,16,6;,
  3;16,15,6;,
  3;12,3,5;,
  3;8,10,5;;
 }
 MeshTextureCoords {
  11;
  0.288850;0.602520;,
  0.274320;0.511390;,
  0.236250;0.602520;,
  0.236250;0.602520;,
  0.288850;0.602520;,
  0.291290;0.511390;,
  0.238700;0.511390;,
  0.282810;0.511390;,
  0.256510;0.511390;,
  0.243150;0.511390;,
  0.289170;0.511390;;
 }
}
