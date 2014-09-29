
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  iOS User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2013, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  As an exception, when this program is distributed through (i) the     *
 *  App Store by Apple Inc.; (ii) the Mac App Store by Apple Inc.; or     *
 *  (iii) Google Play by Google Inc., then that store may impose any      *
 *  digital rights management, device limits and/or redistribution        *
 *  restrictions that are required by its terms of service.               *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

#import "Coordinates.h"
#import "SurfacesMatching.h"
#import "SurfacesViewController.h"
#import "MDSpreadViewClasses.h"
#import "maths/nmatrixint.h"
#import "surfaces/nnormalsurfacelist.h"
#import "triangulation/ntriangulation.h"

// TODO: Listen on triangulation rename.  For all surface tabs, actually.
// TODO: Offer a "compact view".
// TODO: Make a base clase for dealing with MDSpreadView.

// These fonts are hard-coded into the MDSpreadView classes.
// Replicate them here so we can compute cell sizes.
static UIFont* cellFont = [UIFont systemFontOfSize:16];
static UIFont* headerFont = [UIFont boldSystemFontOfSize:14];

// MDSpreadViewHeaderCell.m uses a horizontal padding of 28, assuming no sort indicator.
// Both the header and cell classes use a vertical padding of 3.
#define CELL_WIDTH_PADDING 32
#define CELL_HEIGHT_PADDING 6

@interface SurfacesMatching () <MDSpreadViewDataSource, MDSpreadViewDelegate, PacketDelegate> {
    CGFloat width, height;
    CGFloat widthHeader;
}
@property (weak, nonatomic) IBOutlet UILabel *header;
@property (weak, nonatomic) IBOutlet UILabel *coords;
@property (weak, nonatomic) IBOutlet UIButton *tri;
@property (weak, nonatomic) IBOutlet MDSpreadView *grid;

@property (strong, nonatomic) SurfacesViewController* viewer;
@property (assign, nonatomic) regina::NNormalSurfaceList* packet;
@property (assign, nonatomic) regina::NMatrixInt* matrix;
@end

@implementation SurfacesMatching

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.viewer = static_cast<SurfacesViewController*>(self.parentViewController);
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    self.packet = self.viewer.packet;
    [self.viewer updateHeader:self.header coords:self.coords tri:self.tri];

    delete self.matrix;
    self.matrix = self.packet->recreateMatchingEquations();

    [self initMetrics];
    self.grid.dataSource = self;
    self.grid.delegate = self;
    self.grid.allowsRowHeaderSelection = YES;
}

- (void)dealloc
{
    delete self.matrix;
}

- (void)initMetrics
{
    NSString* text;

    text = [NSString stringWithFormat:@"%ld.", self.matrix->rows() - 1];
    widthHeader = CELL_WIDTH_PADDING + [text sizeWithAttributes:@{NSFontAttributeName: headerFont}].width;

    text = [Coordinates columnName:self.packet->coords()
                        whichCoord:self.matrix->columns()-1
                               tri:self.packet->getTriangulation()];
    CGSize size = [text sizeWithAttributes:@{NSFontAttributeName: cellFont}];
    width = CELL_WIDTH_PADDING + size.width;
    height = CELL_HEIGHT_PADDING + size.height;
}

#pragma mark - MDSpreadView data source

- (NSInteger)spreadView:(MDSpreadView *)aSpreadView numberOfColumnsInSection:(NSInteger)section
{
    return self.matrix->columns();
}

- (NSInteger)spreadView:(MDSpreadView *)aSpreadView numberOfRowsInSection:(NSInteger)section
{
    return self.matrix->rows();
}

- (id)spreadView:(MDSpreadView *)aSpreadView titleForHeaderInRowSection:(NSInteger)section forColumnAtIndexPath:(MDIndexPath *)columnPath
{
    return [Coordinates columnName:self.packet->coords() whichCoord:columnPath.column tri:self.packet->getTriangulation()];
}

- (id)spreadView:(MDSpreadView *)aSpreadView titleForHeaderInColumnSection:(NSInteger)section forRowAtIndexPath:(MDIndexPath *)rowPath
{
    return [NSString stringWithFormat:@"%d.", rowPath.row];
}

- (id)spreadView:(MDSpreadView *)aSpreadView objectValueForRowAtIndexPath:(MDIndexPath *)rowPath forColumnAtIndexPath:(MDIndexPath *)columnPath
{
    regina::NLargeInteger entry = self.matrix->entry(rowPath.row, columnPath.column);
    if (entry.isZero())
        return @"";
    else
        return @(entry.stringValue().c_str());
}

#pragma mark - MDSpreadView delegate

- (CGFloat)spreadView:(MDSpreadView *)aSpreadView widthForColumnHeaderInSection:(NSInteger)columnSection
{
    return widthHeader;
}

- (CGFloat)spreadView:(MDSpreadView *)aSpreadView widthForColumnAtIndexPath:(MDIndexPath *)indexPath
{
    return width;
}

- (CGFloat)spreadView:(MDSpreadView *)aSpreadView heightForRowAtIndexPath:(MDIndexPath *)indexPath
{
    return height;
}

@end
