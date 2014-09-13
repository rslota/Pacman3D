#include "Board.h"
#include "make_ref.h"

#include <cassert>
#include <osg/ShapeDrawable>
#include <osg/Geode>

Board::Board(std::vector<std::string>& def, double sizeX, double sizeY)
    : m_sizeX(sizeX)
    , m_sizeY(sizeY)
{
    assert(def.size() > 0);
    m_fieldMap.resize(def.size());

    auto height = def.size();
    auto width = def[0].size();

    for(auto elem : def) {
        assert(elem.size() >= width);
    }

    for(auto& elem : m_fieldMap) {
        elem.resize(width, false);
    }

    for(auto i = 0; i < height; ++i)
        for(auto j = 0; j < width; ++j)
            m_fieldMap[i][j] = def[i][j] == '*';
}

double Board::getSizeX() const
{
    return m_sizeX;
}

double Board::getSizeY() const
{
    return m_sizeY;
}

uint32_t Board::getFieldCountX() const
{
    return m_fieldMap[0].size();
}

uint32_t Board::getFieldCountY() const
{
    return m_fieldMap.size();
}

double Board::getFieldSizeX() const
{
    return m_sizeX / getFieldCountX();
}

double Board::getFieldSizeY() const
{
    return m_sizeY / getFieldCountY();
}

double Board::getFieldCenterX(uint32_t x) const
{
    return getFieldSizeX() / 2 + x * getFieldSizeX();
}

double Board::getFieldCenterY(uint32_t y) const
{
    return getSizeY() - getFieldSizeY() / 2 - y * getFieldSizeY();
}

osg::ref_ptr<osg::Group> Board::draw() const
{
    auto normalize = osg::Vec3d(0.0f, 0.0f, 0.0f);

    double blockSizeZ = 5 * std::max(getFieldSizeX(), getFieldSizeY());

    auto overlaySize = std::min(getFieldSizeX(), getFieldSizeY()) / 100;

    auto boardObj = make_ref<osg::Group>();

    for(auto y = 0; y < getFieldCountY(); ++y) { // y
        for(auto x = 0; x < getFieldCountX(); ++x) { // x
            if(m_fieldMap[y][x]) {
                auto center = osg::Vec3d(getFieldCenterX(x), getFieldCenterY(y), blockSizeZ / 2) - normalize;
                auto dr = make_ref<osg::ShapeDrawable>();
                dr->setShape(new osg::Box(center, getFieldSizeX() + overlaySize, getFieldSizeY() + overlaySize, blockSizeZ));



                dr->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
                auto drgd = make_ref<osg::Geode>();
                drgd->addDrawable(dr);

                boardObj->addChild(drgd.get());
            }
        }
    }

    auto floor = make_ref<osg::ShapeDrawable>();
    floor->setShape( new osg::Box( osg::Vec3(getSizeX() / 2, getSizeY() / 2, 0.0f) - normalize, getSizeX(), getSizeY(), 0.5f) );
    auto floor_geode = make_ref<osg::Geode>();
    floor_geode->addDrawable(floor);

    boardObj->addChild(floor_geode);

    return boardObj;
}