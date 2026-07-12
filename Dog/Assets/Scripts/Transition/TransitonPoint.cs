using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TransitonPoint : MonoBehaviour
{
    public enum TransitionType
    {
        SameScene, DifferentScene
    }

    [Header("Transition Info")]
    public string sceneName;
    public TransitionType transitionType;

    public TransitionDestination.DestinationTag destinationTag;

    private bool canTrans;

    void Update()
    {
        if(Input.GetKeyDown(KeyCode.E) && canTrans)
        {
            SceneController.Instance.TransitionToDestination(this);
        }
    }

    void OnTriggerStay(Collider other)
    {
        if(CompareTag("Player"))
            canTrans = true;
    }

    void OnTriggerExit(Collider other)
    {
        if (CompareTag("Player"))
            canTrans = false;
    }
}
