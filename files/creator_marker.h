#pragma once

#include <map>
#include <string>
#include <functional>
#include <memory>

#include "markers.h"

using Creator = std::function<std::unique_ptr<Marker>(Section &section)>;

class CreatorMarker {
public:
    CreatorMarker() {}

    void AddCreatorMarker(const std::string& name, Creator creator);
    std::unique_ptr<Marker> CreateMarker(const std::string& name, Section &section);
private:
    std::map<std::string, Creator> creators_;
};