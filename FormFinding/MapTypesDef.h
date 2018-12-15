#pragma once

class Beam;
class Plate;
class PlateProperty;
class BeamProperty;
class LoadPattern;


typedef pair <long, long> longPair;
typedef pair < pair <long, long>, string > bmDistLoadGkey;
typedef std::pair<long, Beam*> mapElement_Beam;
typedef std::pair<long, Node*> mapElement_Node;

typedef std::pair<long, BeamProperty*> mapElement_BeamProperty;
typedef std::pair<long, Plate*> mapElement_Plate;
typedef std::pair<long, PlateProperty*> mapElement_PlateProperty;

typedef std::pair<long, LoadPattern*> mapElement_LoadPattern;

typedef std::pair<longPair, PlPressure*> mapElement_PlatePressure;


