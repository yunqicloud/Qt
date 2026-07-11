using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class PlayerController : MonoBehaviour
{
    private NavMeshAgent agent;

    void Awake()
    {
        agent = GetComponent<NavMeshAgent>();
    }

    void Start()
    {
        MouseManager.Instance.OnMouseClicked += MoveToTarget;
    }

    public void MoveToTarget(Vector3 target)
    {
        agent.destination = target;
    }
}
