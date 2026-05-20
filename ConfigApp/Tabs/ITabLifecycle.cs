namespace ConfigApp.Tabs
{
    internal interface ITabLifecycle
    {
        void OnTabSelected();
        void OnLoadValues();
        void OnSaveValues();
    }
}
