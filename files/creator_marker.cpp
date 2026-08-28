#include "creator_marker.h"

void CreatorMarker::AddCreatorMarker(const std::string& name, Creator creator) {
    creators_[name] = creator;
}

MarkerVariant CreatorMarker::CreateMarker(const std::string& name, Section &section) {
    auto it = creators_.find(name);
    if (it == creators_.end()) {
        //
    }
    return it->second(section);
}