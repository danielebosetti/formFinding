#pragma once


class Node;
class Beam;
class Plate;

// forward declaration for basic categories functions
bool is_NODE_TC_L(Node *n);
bool is_NODE_STEEL(Node *n);
bool is_NODE_I(Node *n);
bool is_NODE_RESTRAINED(Node *n);
bool is_NODE_TC(Node *n);
bool is_NODE_RL(Node *n);
bool is_ELEM_STEEL(Beam *b);
bool is_ELEM_TC_SOAP(Beam *b);
bool is_ELEM_TC_SOAP(Plate *p);
bool is_ELEM_RL(Beam *b);
bool is_ELEM_TC_CE(Beam *b);
bool is_BEAM_for_REAZ_V(Beam *b);
bool is_BEAM_TC(Beam *b);
bool is_PLATE_TC(Plate *p);

