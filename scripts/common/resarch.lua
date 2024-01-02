--- @author Dinnajad

local order = mjrequire "common/order"
local locale = mjrequire "common/locale"
local resource = mjrequire "common/resource"

local mod = {
    loadOrder = 1,
}
function mod:onload(research)
    local super_load = research.load
    research.load = function(research_, gameObject, constructable, flora)
        super_load(research_, gameObject, constructable, flora)

        -- Drying mat
        research:addResarch("weaving",{
            skillTypeIndex = skill.types.spinning.index, --using spinning as a model for weaving since it is pretty similar
            constructableTypeIndexesByBaseResourceTypeIndex = {
                [resource.types.flaxDried.index] = constructable.types.flaxTwine.index,  -- need to change to another outcome
            },
            resourceTypeIndexes = {
                resource.types.flaxDried.index,
            },
            initialResearchSpeedLearnMultiplier = 0.25,
        })
    end
end