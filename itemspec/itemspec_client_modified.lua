--- ______ _   _ _   _   _    ___    _  
--- | ____| | | | \ | |/ _ \ |_ _|  / \
--- |  _| | | | |  \| | | | | | |  / _ \ 
--- | |___| |_| | |\  | |_| | | | / ___ \
--- |_____|\___/ _| \_|\___/ |___/_/   \_\
---     https://discord.gg/azrv6brSgh
---       MODIFIED FOR SPELL-BASED SYSTEM

-- Spell IDs for communication
local SPELL_OPEN_UI = 99990
local SPELL_SAVE_SPEC_1 = 99991
local SPELL_SAVE_SPEC_2 = 99992
local SPELL_SAVE_SPEC_3 = 99993
local SPELL_GET_CURRENT = 99994

------------------------------------------------------------------------------------------------------------------------------------------
-- ShowMenu
------------------------------------------------------------------------------------------------------------------------------------------

local function ShowMenu()
    if not ItemSpecFrame then
        print("ERROR: ItemSpecFrame is nil!")
        return
    end

    ItemSpecFrame:Show()
    
    -- Request current spec from server
    CastSpellByID(SPELL_GET_CURRENT)
end

------------------------------------------------------------------------------------------------------------------------------------------
-- Chat Message Listener
------------------------------------------------------------------------------------------------------------------------------------------

local chatFrame = CreateFrame("Frame")
chatFrame:RegisterEvent("CHAT_MSG_SYSTEM")
chatFrame:SetScript("OnEvent", function(self, event, msg)
    -- Listen for server responses
    if msg:find("ITEMSPEC_DATA:") then
        local spec = msg:match("ITEMSPEC_DATA:(.+)")
        if spec then
            -- Update UI with current spec
            if ItemSpecFrame and ItemSpecFrame:IsShown() then
                local infoText = ItemSpecFrame.infoText
                if infoText then
                    infoText:SetText("Current Saved Spec: |cff00ff00" .. spec .. "|r")
                end
                
                -- Highlight the matching button
                if ItemSpecFrame.buttonRefs then
                    for _, ref in pairs(ItemSpecFrame.buttonRefs) do
                        if ref.label:GetText() == spec then
                            ref.label:SetTextColor(0, 1, 0)
                        else
                            ref.label:SetTextColor(1, 1, 1)
                        end
                    end
                end
            end
        end
    end
end)

------------------------------------------------------------------------------------------------------------------------------------------
-- Main Frame
------------------------------------------------------------------------------------------------------------------------------------------

ItemSpecFrame = CreateFrame("Frame", "ItemSpecFrame", UIParent)
ItemSpecFrame:SetSize(850, 650)
ItemSpecFrame:SetPoint("CENTER")
ItemSpecFrame:SetMovable(true)
ItemSpecFrame:EnableMouse(true)
ItemSpecFrame:RegisterForDrag("LeftButton")
ItemSpecFrame:SetScript("OnDragStart", ItemSpecFrame.StartMoving)
ItemSpecFrame:SetScript("OnDragStop", ItemSpecFrame.StopMovingOrSizing)

local ItemSpecFrameContainer = ItemSpecFrame:CreateTexture(nil, "OVERLAY")
ItemSpecFrameContainer:SetTexture("Interface\\Eunoia\\WeeklyReward\\evergreenweeklyrewardui")
ItemSpecFrameContainer:SetTexCoord(-0.0036621094, 0.5744629, 0.35180664, 0.6687012)
ItemSpecFrameContainer:SetAllPoints()

local ItemSpecFrameBackground = ItemSpecFrame:CreateTexture(nil, "BACKGROUND")
ItemSpecFrameBackground:SetTexture("Interface\\Eunoia\\WeeklyReward\\evergreenweeklyrewardui")
ItemSpecFrameBackground:SetTexCoord(-0.00024414062, 0.55444336, -0.0007324219, 0.34692383)
ItemSpecFrameBackground:SetPoint("CENTER", ItemSpecFrame, "CENTER", 0, 0)
ItemSpecFrameBackground:SetSize(825, 625)

local close = CreateFrame("Button", nil, ItemSpecFrame, "UIPanelCloseButton")
close:SetPoint("TOPRIGHT", ItemSpecFrame, "TOPRIGHT", 0, -2)
close:SetScript("OnClick", function()
    ItemSpecFrame:Hide()
end)

ItemSpecFrame.title = ItemSpecFrame:CreateFontString(nil, "OVERLAY", "GameFontHighlightLarge")
ItemSpecFrame.title:SetPoint("TOP", ItemSpecFrame, "TOP", 0, -10)
ItemSpecFrame.title:SetText("")

local ItemSpecLogo = ItemSpecFrame:CreateTexture(nil, "ARTWORK")
ItemSpecLogo:SetSize(300, 180)
ItemSpecLogo:SetPoint("TOP", ItemSpecFrame, "TOP", 0, -15)
ItemSpecLogo:SetTexture("Interface\\Glues\\COMMON\\Glues-WoW-WotLKLogo")
ItemSpecLogo:SetAlpha(0.7)

local ItemSpecDivider = ItemSpecFrame:CreateTexture(nil, "OVERLAY")
ItemSpecDivider:SetSize(800, 20)
ItemSpecDivider:SetPoint("CENTER", ItemSpecFrame, "CENTER", 0, 148)
ItemSpecDivider:SetTexture("Interface\\Eunoia\\WeeklyReward\\evergreenweeklyrewardui")
ItemSpecDivider:SetTexCoord(0.41723633, 0.77563477, 0.70239258, 0.71362305)
ItemSpecDivider:SetAlpha(0.8)

ItemSpecFrame:Hide()

------------------------------------------------------------------------------------------------------------------------------------------
-- Item Spec UI
------------------------------------------------------------------------------------------------------------------------------------------

local title = ItemSpecFrame:CreateFontString(nil, "OVERLAY", "GameFontHighlightLarge")
title:SetPoint("TOP", ItemSpecFrame, "TOP", 0, -205)
title:SetText("Select Your Specialization")

ItemSpecFrame:SetScript("OnShow", function()
    CastSpellByID(SPELL_GET_CURRENT)
end)

local ClassSpecs = {
    [1]  = {"Arms", "Fury", "Protection"},
    [2]  = {"Holy", "Protection", "Retribution"},
    [3]  = {"Beast Mastery", "Marksmanship", "Survival"},
    [4]  = {"Assassination", "Combat", "Subtlety"},
    [5]  = {"Discipline", "Holy", "Shadow"},
    [6]  = {"Blood", "Frost", "Unholy"},
    [7]  = {"Elemental", "Enhancement", "Restoration"},
    [8]  = {"Arcane", "Fire", "Frost"},
    [9]  = {"Affliction", "Demonology", "Destruction"},
    [11] = {"Balance", "Feral", "Restoration"}
}

local SpecBLPs = {
    [1] = {
        ["Arms"] = "Interface\\EunoiaMenu\\Specs\\bg-warrior-arms",
        ["Fury"] = "Interface\\EunoiaMenu\\Specs\\bg-warrior-fury",
        ["Protection"] = "Interface\\EunoiaMenu\\Specs\\bg-warrior-protection",
    },
    [2] = {
        ["Holy"] = "Interface\\EunoiaMenu\\Specs\\bg-paladin-holy",
        ["Protection"] = "Interface\\EunoiaMenu\\Specs\\bg-paladin-protection",
        ["Retribution"] = "Interface\\EunoiaMenu\\Specs\\bg-paladin-retribution",
    },
    [3] = {
        ["Beast Mastery"] = "Interface\\EunoiaMenu\\Specs\\bg-hunter-beastmaster",
        ["Marksmanship"] = "Interface\\EunoiaMenu\\Specs\\bg-hunter-marksman",
        ["Survival"] = "Interface\\EunoiaMenu\\Specs\\bg-hunter-survival",
    },
    [4] = {
        ["Assassination"] = "Interface\\EunoiaMenu\\Specs\\bg-rogue-assassination",
        ["Combat"] = "Interface\\EunoiaMenu\\Specs\\bg-rogue-combat",
        ["Subtlety"] = "Interface\\EunoiaMenu\\Specs\\bg-rogue-subtlety",
    },
    [5] = {
        ["Discipline"] = "Interface\\EunoiaMenu\\Specs\\bg-priest-discipline",
        ["Holy"] = "Interface\\EunoiaMenu\\Specs\\bg-priest-holy",
        ["Shadow"] = "Interface\\EunoiaMenu\\Specs\\bg-priest-shadow",
    },
    [6] = {
        ["Blood"] = "Interface\\EunoiaMenu\\Specs\\bg-deathknight-blood",
        ["Frost"] = "Interface\\EunoiaMenu\\Specs\\bg-deathknight-frost",
        ["Unholy"] = "Interface\\EunoiaMenu\\Specs\\bg-deathknight-unholy",
    },
    [7] = {
        ["Elemental"] = "Interface\\EunoiaMenu\\Specs\\bg-shaman-elemental",
        ["Enhancement"] = "Interface\\EunoiaMenu\\Specs\\bg-shaman-enhancement",
        ["Restoration"] = "Interface\\EunoiaMenu\\Specs\\bg-shaman-restoration",
    },
    [8] = {
        ["Arcane"] = "Interface\\EunoiaMenu\\Specs\\bg-mage-arcane",
        ["Fire"] = "Interface\\EunoiaMenu\\Specs\\bg-mage-fire",
        ["Frost"] = "Interface\\EunoiaMenu\\Specs\\bg-mage-frost",
    },
    [9] = {
        ["Affliction"] = "Interface\\EunoiaMenu\\Specs\\bg-warlock-affliction",
        ["Demonology"] = "Interface\\EunoiaMenu\\Specs\\bg-warlock-demonology",
        ["Destruction"] = "Interface\\EunoiaMenu\\Specs\\bg-warlock-destruction",
    },
    [11] = {
        ["Balance"] = "Interface\\EunoiaMenu\\Specs\\bg-druid-balance",
        ["Feral"] = "Interface\\EunoiaMenu\\Specs\\bg-druid-cat",
        ["Restoration"] = "Interface\\EunoiaMenu\\Specs\\bg-druid-restoration",
    }
}

local _, class = UnitClass("player")
local classID = 0

if class == "WARRIOR" then classID = 1
elseif class == "PALADIN" then classID = 2
elseif class == "HUNTER" then classID = 3
elseif class == "ROGUE" then classID = 4
elseif class == "PRIEST" then classID = 5
elseif class == "DEATHKNIGHT" then classID = 6
elseif class == "SHAMAN" then classID = 7
elseif class == "MAGE" then classID = 8
elseif class == "WARLOCK" then classID = 9
elseif class == "DEMONHUNTER" then classID = 10
elseif class == "DRUID" then classID = 11
else
    classID = 1
    print("Failed to detect your class, fallbacking to Warrior")
end

local specs = ClassSpecs[classID]
local buttonRefs = {}

local startX = -260
local spacing = 257

for i, specName in ipairs(specs) do
    local btn = CreateFrame("Button", nil, ItemSpecFrame)
    btn:SetSize(250, 300)
    btn:SetPoint("CENTER", ItemSpecFrame, "CENTER", startX + (i - 1) * spacing, -80)

    local bg = btn:CreateTexture(nil, "BACKGROUND")
    bg:SetAllPoints(btn)
    bg:SetTexture((SpecBLPs[classID][specName]) or "Interface\\DialogFrame\\UI-DialogBox-Background-Dark")
    bg:SetTexCoord(0.0009765625, 0.98535156, 0.0009765625, 0.7001953)
    
    local label = btn:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
    label:SetPoint("CENTER", btn, "CENTER", 0, -100)
    label:SetText(specName)

    btn:SetScript("OnClick", function()
        -- Cast the appropriate spell based on spec index
        if i == 1 then
            CastSpellByID(SPELL_SAVE_SPEC_1)
        elseif i == 2 then
            CastSpellByID(SPELL_SAVE_SPEC_2)
        elseif i == 3 then
            CastSpellByID(SPELL_SAVE_SPEC_3)
        end
        
        -- Update UI immediately
        for _, ref in pairs(buttonRefs) do
            ref.label:SetTextColor(1, 1, 1)
        end
        label:SetTextColor(0, 1, 0)
    end)

    buttonRefs[i] = {btn = btn, label = label}
end

-- Store button refs for chat listener
ItemSpecFrame.buttonRefs = buttonRefs

-- Info text
local infoText = ItemSpecFrame:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
infoText:SetPoint("BOTTOM", ItemSpecFrame, "BOTTOM", 0, 30)
infoText:SetText("Loading current spec...")
ItemSpecFrame.infoText = infoText

------------------------------------------------------------------------------------------------------------------------------------------
-- Slash Command
------------------------------------------------------------------------------------------------------------------------------------------

SLASH_ITEMSPEC1 = "/itemspec"
SlashCmdList["ITEMSPEC"] = function()
    ShowMenu()
end

------------------------------------------------------------------------------------------------------------------------------------------
-- Optional: Item to open UI
------------------------------------------------------------------------------------------------------------------------------------------

-- If you create an item with ID 50000 that casts spell 99990, using it will open the UI
-- No additional code needed on client side!

print("|cff00ff00ItemSpec Loaded!|r Type /itemspec to open, or cast spell 99990")