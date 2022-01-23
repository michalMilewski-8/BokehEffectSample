try
{
    if (model && model.skyBoxes)
    {
        model.skyBoxes.push({text: 'Milkyway by Lula Mae', value: '/pa/sky/terrain/textures/com.pa.lulamae.milkyway-skybox/skybox.json'});
    }
}
catch (e)
{
    console.trace(e);
}