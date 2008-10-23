/**********************************/
/* classes to handle copper zones */
/**********************************/

#ifndef CLASS_ZONE_H
#define CLASS_ZONE_H

#include <vector>

#include "PolyLine.h"


/************************/
/* class ZONE_CONTAINER */
/************************/

/* handle a list of polygons delimiting a copper zone
 * a zone is described by a main polygon, a time stamp, a layer and a net name.
 * others polygons inside this main polygon are holes.
 */

class ZONE_CONTAINER : public BOARD_ITEM
{
public:
    wxString              m_Netname;                        // Net Name
    CPolyLine*            m_Poly;                           // outlines
    int                   m_CornerSelection;                // For corner moving, corner index to drag, or -1 if no selection
    int                   m_ZoneClearance;                  // clearance value
    int                   m_GridFillValue;                  // Grid used for filling, 0 = use polygonal areas to fill
    int                   m_ArcToSegmentsCount;             // number of segments to convert a cirlce to a polygon (uses 16 or 32)
    int                   m_PadOption;                      //
    int                   m_ThermalReliefGapValue;          // tickness of the gap in thermal reliefs
    int                   m_ThermalReliefCopperBridgeValue; // tickness of the copper bridge in thermal reliefs
    int                   utility, utility2;                // flags used in polygon calculations
    std::vector <CPolyPt> m_FilledPolysList;  /* set of filled polygons used to draw a zone as a filled area.
                                               * from outlines (m_Poly) but unlike m_Poly these filled polygons have no hole (they are all in one piece)
                                               * In very simple cases m_FilledPolysList is same as m_Poly
                                               * In less simple cases (when m_Poly has holes) m_FilledPolysList is a polygon equivalent to m_Poly, without holes
                                               * In complex cases an ouline decribed by m_Poly can have many filled areas
                                               */
    int m_DrawOptions;                          /* used to pass some draw options (draw filled areas in sketch mode for instance ...)
                                                 * currently useful when testing filling zones algos
                                                 */

private:
    int m_NetCode; // Net number for fast comparisons

public:
    ZONE_CONTAINER( BOARD* parent );
    ~ZONE_CONTAINER();

    bool Save( FILE* aFile ) const;
    int  ReadDescr( FILE* aFile, int* aLineNum = NULL );

    wxPoint& GetPosition()
    {
        static wxPoint pos;

        return pos;
    }


    void UnLink( void )
    {
    };

    /**
     * Function copy
     * copy usefull data from the source.
     * flags and linked list pointers are NOT copied
     */
    void Copy( ZONE_CONTAINER* src );

    void Display_Infos( WinEDA_DrawFrame* frame );

    /**
     * Function Draw
     * Draws the zone outline.
     * @param panel = current Draw Panel
     * @param DC = current Device Context
     * @param offset = Draw offset (usually wxPoint(0,0))
     * @param aDrawMode = GR_OR, GR_XOR, GR_COPY ..
     */
    void Draw( WinEDA_DrawPanel* panel,
               wxDC*             DC,
               int               aDrawMode,
               const wxPoint&    offset = ZeroOffset );

    /**
     * Function DrawDrawFilledArea
     * Draws the filled  area for this zone (polygon list .m_FilledPolysList)
     * @param panel = current Draw Panel
     * @param DC = current Device Context
     * @param offset = Draw offset (usually wxPoint(0,0))
     * @param aDrawMode = GR_OR, GR_XOR, GR_COPY ..
     */
    void     DrawFilledArea( WinEDA_DrawPanel* panel,
                             wxDC*             DC,
                             int               aDrawMode,
                             const wxPoint&    offset = ZeroOffset );

    EDA_Rect GetBoundingBox();

    /**
     * Function Test_For_Copper_Island_And_Remove__Insulated_Islands
     * Remove insulated copper islands found in m_FilledPolysList.
     * @param aPcb = the board to analyse
     */
    void     Test_For_Copper_Island_And_Remove_Insulated_Islands( BOARD* aPcb );

    /**
     * Function DrawWhileCreateOutline
     * Draws the zone outline when ir is created.
     * The moving edges are in XOR graphic mode, old segment in draw_mode graphic mode (usually GR_OR)
     * The closing edge has its owm shape
     * @param panel = current Draw Panel
     * @param DC = current Device Context
     * @param draw_mode = draw mode: OR, XOR ..
     */
    void     DrawWhileCreateOutline( WinEDA_DrawPanel* panel, wxDC* DC, int draw_mode = GR_OR );


    /**
     * Function IsOnCopperLayer
     * @return true if this zone is on a copper layer, false if on a technical layer
     */
    bool IsOnCopperLayer( void )
    {
        return ( GetLayer() < FIRST_NO_COPPER_LAYER ) ? true : false;
    }


    int GetNet( void ) const
    {
        return m_NetCode;
    }


    void SetNet( int anet_code );

    /**
     * Function SetNetNameFromNetCode
     * Fin the nat name corresponding to the net code.
     * @return bool - true if net found, else false
     */
    bool SetNetNameFromNetCode( void);

    /**
     * Function HitTest
     * tests if the given wxPoint is within the bounds of this object.
     * @param refPos A wxPoint to test
     * @return bool - true if a hit, else false
     */
    bool HitTest( const wxPoint& refPos );

    /** function BuildFilledPolysListData
     * Build m_FilledPolysList data from real outlines (m_Poly)
     * in order to have drawable (and plottable) filled polygons
     * drawable filled polygons are polygons without hole
     * @param aPcb: the current board (can be NULL for non copper zones)
     * @return number of polygons
     * This function does not add holes for pads and tracks but calls
     * AddClearanceAreasPolygonsToPolysList() to do that for copper layers
     */
    int  BuildFilledPolysListData( BOARD* aPcb );

    /** function AddClearanceAreasPolygonsToPolysList
     * Add non copper areas polygons (pads and tracks with clearence)
     * to a filled copper area
     * used in BuildFilledPolysListData when calculating filled areas in a zone
     * Non copper areas are pads and track and their clearance area
     * The filled copper area must be computed before
     * BuildFilledPolysListData() call this function just after creating the
     *  filled copper area polygon (without clearence areas
     * @param aPcb: the current board
     */
    void AddClearanceAreasPolygonsToPolysList( BOARD* aPcb );

    /**
     * Function HitTestForCorner
     * tests if the given wxPoint near a corner, or near the segment define by 2 corners.
     * @return -1 if none, corner index in .corner <vector>
     * @param refPos : A wxPoint to test
     */
    int  HitTestForCorner( const wxPoint& refPos );

    /**
     * Function HitTestForEdge
     * tests if the given wxPoint near a corner, or near the segment define by 2 corners.
     * @return -1 if none,  or index of the starting corner in .corner <vector>
     * @param refPos : A wxPoint to test
     */
    int  HitTestForEdge( const wxPoint& refPos );

    /**
     * Function HitTest (overlayed)
     * tests if the given EDA_Rect contains the bounds of this object.
     * @param refArea : the given EDA_Rect
     * @return bool - true if a hit, else false
     */
    bool HitTest( EDA_Rect& refArea );

    /**
     * Function Fill_Zone()
     * Calculate the zone filling
     * The zone outline is a frontier, and can be complex (with holes)
     * The filling starts from starting points like pads, tracks.
     * If exists the old filling is removed
     * @param frame = reference to the main frame
     * @param DC = current Device Context
     * @param verbose = true to show error messages
     * @return error level (0 = no error)
     */
    int  Fill_Zone( WinEDA_PcbFrame* frame, wxDC* DC, bool verbose = TRUE );

    /* Geometric transformations: */

    /**
     * Function Move
     * Move the outlines
     * @param offset = moving vector
     */
    void Move( const wxPoint& offset );

    /**
     * Function MoveEdge
     * Move the outline Edge. m_CornerSelection is the start point of the outline edge
     * @param offset = moving vector
     */
    void MoveEdge( const wxPoint& offset );

    /**
     * Function Rotate
     * Move the outlines
     * @param centre = rot centre
     * @param angle = in 0.1 degree
     */
    void Rotate( const wxPoint& centre, int angle );

    /**
     * Function Mirror
     * Mirror the outlines , relative to a given horizontal axis
     * the layer is not changed
     * @param mirror_ref = vertical axis position
     */
    void Mirror( const wxPoint& mirror_ref );

    /**
     * Function GetClass
     * returns the class name.
     * @return wxString
     */
    wxString GetClass() const
    {
        return wxT( "ZONE_CONTAINER" );
    }


    /** Acces to m_Poly parameters
     */

    int GetNumCorners( void )
    {
        return m_Poly->GetNumCorners();
    }


    void RemoveAllContours( void )
    {
        m_Poly->RemoveAllContours();
    }


    wxPoint GetCornerPosition( int aCornerIndex )
    {
        return wxPoint( m_Poly->GetX( aCornerIndex ), m_Poly->GetY( aCornerIndex ) );
    }


    void SetCornerPosition( int aCornerIndex, wxPoint new_pos )
    {
        m_Poly->SetX( aCornerIndex, new_pos.x );
        m_Poly->SetY( aCornerIndex, new_pos.y );
    }


    void AppendCorner( wxPoint position )
    {
        m_Poly->AppendCorner( position.x, position.y );
    }

    int GetHatchStyle() const
    {
        return m_Poly->GetHatchStyle();
    }
};


#endif  // #ifndef CLASS_ZONE_H
