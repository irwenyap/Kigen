using ScriptCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    public class ParallaxBackground : Entity
    {
        //public ParallaxLayer[] parallaxLayers = new ParallaxLayer[6]; //Fixed Array because we only have 6 layers
        ////public List<ParallaxLayer> parallaxLayers = new List<ParallaxLayer>();

        //protected override void OnInit()
        //{
        //    SetLayers();
        //}

        //public void SetLayers()
        //{
        //    parallaxLayers[0] = FindEntityByName("Layer-1").As<ParallaxLayer>();
        //    if (parallaxLayers[0] != null ) { Console.WriteLine("Layer-1 Set"); }

        //    parallaxLayers[1] = FindEntityByName("Layer-2").As<ParallaxLayer>();
        //    if (parallaxLayers[1] != null) { Console.WriteLine("Layer-2 Set"); }

        //    parallaxLayers[2] = FindEntityByName("Layer-3").As<ParallaxLayer>();
        //    if (parallaxLayers[2] != null) { Console.WriteLine("Layer-3 Set"); }

        //    parallaxLayers[3] = FindEntityByName("Layer-4").As<ParallaxLayer>();
        //    if (parallaxLayers[3] != null) { Console.WriteLine("Layer-4 Set"); }

        //    parallaxLayers[4] = FindEntityByName("Layer-5").As<ParallaxLayer>();
        //    if (parallaxLayers[4] != null) { Console.WriteLine("Layer-5 Set"); }

        //    parallaxLayers[5] = FindEntityByName("Layer-6").As<ParallaxLayer>();
        //    if (parallaxLayers[5] != null) { Console.WriteLine("Layer-6 Set"); }
        //}

        //public void Move(float delta)
        //{
        //    for(int i = 0; i < 6; i++)
        //    {
        //        if (parallaxLayers[i] != null)
        //        {
        //            Console.WriteLine($"Moving {parallaxLayers[i]}");
        //            parallaxLayers[i].MoveLayer(delta);
        //        }
        //    }
        //}
    }
}
