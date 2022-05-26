
Files = 
{
	"Script/UI/Image_BackGround.lua",
	"Script/UI/Label_Seed.lua",
	"Script/UI/Label_Title.lua",
	"Script/UI/Label_Version.lua",
	"Script/UI/TextInput_Seed.lua"
}

function LoadUI()
	for _, filePath in ipairs(Files) do
		CPP_LoadUIElement(filePath)
		--print("Load"..filePath.."\n")
	end
end

    