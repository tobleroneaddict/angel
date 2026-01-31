#include "angel.h"

int main()
{
    Bundle assets;

    /*
    This example works with https://models.spriters-resource.com/media/assets/301/304261.zip?updated=1755501322
    Extract the contents of Hachune Miku Doll into a directory, and put the 8 files into a root of a tar.gz.
    rename the [name].tar.gz to assets.tar.gz.tns, and put it in the root of your calculator along with with AnGL-example.tns.
    Of course you dont have to use miku, follow along for using your own assets!
    */


    /*
    AnGL uses tar.gz files for storing video game assets, including but not limited to Wavefront object files, textures, text, or possibly even sound.
    This library was created with nGL compatiblity first, so it contains the framework to load your 3D models.
    By storing files separately on a tarball, you can reduce the size of a program drastically (And it's compile size) opposed to using nGL's python converter. (Sorry Vogt)

    Only OBJ files with PNG textures are supported for now.

    For your development, it is recommended (but not required) to use the firebird emulator https://github.com/nspire-emus/firebird because this library uses alot of printf() debug statements at the moment.


    */
    
    /*
    This is an example of how assets are loaded. assets.tar.gz.tns exists on the root of the calculator, as with nGL.

    This can also be unloaded using assets.free() after loading all the textures.
    Note that once assets are freed, no groups can be loaded until you load another asset bundle.
    Once a group has been loaded, it does not depend on the asset bundle anymore.

    SAMPLE:
    textures/	Size: 0 bytes
    textures/roomitems_013_color.png	Size: 39839 bytes
    textures/roomitems_013_alpha.png	Size: 4907 bytes
    ROOMITEMS013_ALL.mtl	Size: 701 bytes
    ROOMITEMS013_ALL.obj	Size: 121986 bytes
    roomitem_s_kage.png	Size: 10292 bytes

    IMPORTANT! If you decide to separate say, your textures, append the folder name in the .mtl before transfer.
    Especially for a model sourced from the internet.

    map_Kd roomitems_013_color.png
        BECOMES
    map_Kd textures/roomitems_013_color.png

    And your textures should work, even in a folder!

    */
    if (assets.load_asset_bundle("assets.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }

    //All assets in a file can be viewed this way. This includes every file in the tarball.
    printf("Assets:\n");
    assets.debug_list_assets();
    printf("----\n");

    //ModelGroups contain multiple objects. A house would have a foundation, walls, and a roof. They would all be contained in this group as objects.
    ModelGroup group;
    
    //Define the name of the obj file to load into your group. This can be modified in a program like blender, or in the .obj itself. 
    //notice: .obj extension is ommited! house.obj becomes house, as house.mtl and house.png are all likely to coexist. and make it easier to keep track in my head
    //Objects and groups in the .obj file are treated as *the same* (for now).
    
    if (group.load_group(&assets,"ROOMITEMS013_ALL")) return 1;

    printf("Objects in group:\n");
    group.debug_list_objects();
    printf("----\n");

    //Load our objects from the group
    ngl_object* cube = group.get_object("_0_0");
    ngl_object* base1 = group.get_object("_1_0");
    ngl_object* base2 = group.get_object("_3_0");

    //Neat way to store all ngl_objects.
    const ngl_object *scene[] = {
        cube,
        base1,
        base2
    };
    for(auto &&obj : scene) {if (obj == nullptr) { return 1;} } //Exit if any objects are missing
    
    /* VOGTINATOR CODE */

    // Initialize nGL first
    nglInit();
    // Allocate the framebuffer
    TEXTURE *screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT, 0, false);
    nglSetBuffer(screen->bitmap);

    GLFix angle = 0;

    size_t max_pos = 0;
    for(auto &&obj : scene)
        if(obj->count_positions > max_pos)
            max_pos = obj->count_positions;

    ProcessedPosition *processed = new ProcessedPosition[max_pos];

    #ifdef _TINSPIRE
    while(!isKeyPressed(KEY_NSPIRE_ESC))
    #else
    for(unsigned int i = 1300;--i;)
    #endif
    {
        glPushMatrix();
		glColor3f(0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glTranslatef(0, -220, 600);
		angle += 1;
		nglRotateY(angle.normaliseAngle());

        glScale3f(100, 100, 100);

        bool transformed = false;
        for(auto &&obj : scene) //Loop through AnGL scene
        {
            glBindTexture(obj->texture);
            nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode, !transformed);
            transformed = true;
        }

        glPopMatrix();
        nglDisplay();
    }

    group.free_group();

    delete[] processed;
    // Deinitialize nGL
    nglUninit();
    deleteTexture(screen);

    return 0;
}