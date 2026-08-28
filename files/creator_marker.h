#pragma once

#include <map>
#include <string>
#include <functional>
#include <memory>
#include <variant>

#include "markers.h"

using MarkerVariant = std::variant<TableQuant, Sof0, Dht, Sos>;
using Creator = std::function<MarkerVariant(Section &section)>;

class CreatorMarker {
public:
    CreatorMarker() {}

    void AddCreatorMarker(const std::string& name, Creator creator);
    MarkerVariant CreateMarker(const std::string& name, Section &section);
private:
    std::map<std::string, Creator> creators_;
};