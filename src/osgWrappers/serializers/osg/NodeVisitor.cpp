#undef OBJECT_CAST
#define OBJECT_CAST dynamic_cast

#include <osg/NodeVisitor>
#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

/// A convenience method to get the simulation time.
/// Returns 0.0 if the framestamp is not available.
/// TODO replace this with full access to the FrameStamp.
struct NodeVisitorGetSimulationTime : public osgDB::MethodObject
{
    virtual bool run(   void* objectPtr
                      , osg::Parameters& //inputParameters
                      , osg::Parameters& outputParameters) const
    {
        double sim_time = 0.0;

        // The actual visitor might be something other than an
        // osg::NodeVisitor, e.g., an osgUtil::UpdateVisitor.
        // However, this is called (indirectly) via an osg::CallbackObject.
        // osg::CallbackObject::run() takes an osg::Object*, so the objectPtr
        // here is actually osg::Object*.  Therefore, we can safely
        // reinterpret_cast to that.  Then we can dynamic_cast to
        // osg::NodeVisitor* regardless of the concrete type of NodeVisitor.
        osg::NodeVisitor* nv =
            dynamic_cast<osg::NodeVisitor*>(
                    reinterpret_cast<osg::Object*>(objectPtr)
            );

        if(nv)
        {
            const osg::FrameStamp *fs = nv->getFrameStamp();
            if(fs) sim_time = fs->getSimulationTime();
        }

        outputParameters.push_back(
            new osg::DoubleValueObject("return", sim_time));
        return true;
    }
};


/// The object wrapper for NodeVisitor
REGISTER_OBJECT_WRAPPER( NodeVistor,
                         new osg::NodeVisitor,
                         osg::NodeVisitor,
                         "osg::Object osg::NodeVisitor" )
{
    // Serialization
    BEGIN_ENUM_SERIALIZER( TraversalMode, TRAVERSE_NONE );
        ADD_ENUM_VALUE( TRAVERSE_NONE );
        ADD_ENUM_VALUE( TRAVERSE_PARENTS );
        ADD_ENUM_VALUE( TRAVERSE_ALL_CHILDREN );
        ADD_ENUM_VALUE( TRAVERSE_ACTIVE_CHILDREN );
    END_ENUM_SERIALIZER();

    BEGIN_ENUM_SERIALIZER( VisitorType, NODE_VISITOR );
        ADD_ENUM_VALUE( UPDATE_VISITOR );
        ADD_ENUM_VALUE( EVENT_VISITOR );
        ADD_ENUM_VALUE( COLLECT_OCCLUDER_VISITOR );
        ADD_ENUM_VALUE( CULL_VISITOR );
        ADD_ENUM_VALUE( INTERSECTION_VISITOR );
    END_ENUM_SERIALIZER();

    ADD_UINT_SERIALIZER(TraversalMask, 0xffffffff);
    ADD_UINT_SERIALIZER(TraversalNumber, 0);

    // Custom methods
    ADD_METHOD_OBJECT( "getSimulationTime", NodeVisitorGetSimulationTime );
}


#undef OBJECT_CAST
#define OBJECT_CAST static_cast
