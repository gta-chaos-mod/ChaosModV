namespace ConfigApp.Tabs
{
    internal interface ITabLifecycle
    {
        void OnLoadValues();
        void OnSaveValues();
        void OnTabSelected();
    }
}
