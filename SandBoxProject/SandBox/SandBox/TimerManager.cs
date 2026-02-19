using ScriptCore;
using System;
using System.Collections.Generic;

namespace SandBox
{
    public class TimerManager : Entity
    {
        //private List<Timer> activeTimers = new List<Timer>();
        //private List<Coroutine> activeCoroutines = new List<Coroutine>();
        //private float time = 0f;
        //protected override void OnUpdate(float dt)
        //{
        //    time += dt;
        //    if (Input.IsKeyPressed(KeyCode.N))
        //    {
        //        StartCoroutine(2f, (float deltaTime) => Logger.Log("Running coroutine, deltaTime is " + deltaTime, LogLevel.INFO));
        //    }

        //    foreach (Timer timer in activeTimers)
        //    {
        //        if (time >= timer.length)
        //        {
        //            timer.action?.Invoke();
        //            activeTimers.Remove(timer);
        //            Logger.Log("Ended timer", LogLevel.INFO);
        //        }
        //    }
        //    foreach (Coroutine coroutine in activeCoroutines)
        //    {
        //        if (coroutine.length >= time)
        //        {
        //            if (coroutine.action != null) coroutine.action(dt);
        //        }
        //        else
        //        {
        //            activeCoroutines.Remove(coroutine);
        //            Logger.Log("Ended coroutine", LogLevel.INFO);
        //        }
        //    }
        //}
        //public void StartTimer(float length, Action action)
        //{
        //    Timer timer = new Timer(time + length, action);

        //    Logger.Log($"Started timer, time is {time} timer ending at {timer.length}", LogLevel.INFO);

        //    if (timer != null) activeTimers?.Add(timer);
        //}
        //public void StartCoroutine(float length, Action<float> action)
        //{
        //    Coroutine timer = new Coroutine(time + length, action);

        //    Logger.Log($"Started coroutine, time is {time} coroutine ending at {timer.length}", LogLevel.INFO);

        //    if (timer != null) activeCoroutines?.Add(timer);
        //}
    }
    //public class Timer
    //{
    //    public float length;
    //    public Action action;
    //    public Timer(float length = 0f, Action action = null)
    //    {
    //        this.length = length;
    //        if (action != null) this.action = action;
    //    }
    //}
    //public class Coroutine
    //{
    //    public float length;
    //    public Action<float> action;
    //    public Coroutine(float length = 0f, Action<float> action = null)
    //    {
    //        this.length = length;
    //        if (action != null) this.action = action;
    //    }
    //}
}
